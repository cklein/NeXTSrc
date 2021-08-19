/*
 * bootstrap -- fundamental service initiator and port server
 * Mike DeMoney, NeXT, Inc.
 * Copyright, 1990.  All rights reserved.
 *
 * rpc_services.c -- implementation of bootstrap rpc services
 */

#import "bootstrap_internal.h"
#import "error_log.h"
#import "lists.h"
#import "bootstrap.h"

#import <mach.h>
#import <string.h>

#ifndef NULL
#define	NULL	((void *)0)
#endif NULL
 
extern port_all_t backup_port;

/*
 * Old mach_init(aka service) compatability routines
 */
kern_return_t
x_service_checkin(
	port_t		bootstrap_port,
	port_t		service_desired,
	port_all_t	*service_granted)
{
	service_t *servicep;
	
	servicep = lookup_service_by_port(service_desired);
	if (   servicep != NULL
	    && servicep->mach_port_num != NO_MACH_PORT
	    && !servicep->isActive)
	{
		*service_granted = service_desired;
		ASSERT(!servicep->isActive);
		servicep->isActive = TRUE;
		info("service_checkin port %d succeeded", service_desired);
		return KERN_SUCCESS;
	}
	debug("service_checkin port %d failed", service_desired);
	*service_granted = PORT_NULL;
	return KERN_FAILURE;
}
	
kern_return_t
x_service_status(
	port_t		bootstrap_port,
	port_t		service_desired,
	boolean_t	*isrunning)
{
	service_t *servicep;
	
	servicep = lookup_service_by_port(service_desired);
	if (   servicep != NULL
	    && servicep->mach_port_num != NO_MACH_PORT)
	{
		*isrunning = ! canReceive(service_desired);
#if	DEBUG
		debug("service_status port %d (%s) %s",
			service_desired, servicep->name,
			*isrunning ? "running" : "idle");
#endif	DEBUG
		return KERN_SUCCESS;
	}
#if	DEBUG
	debug("service_status port %d unknown", service_desired);
#endif	DEBUG
	return KERN_FAILURE;
}	 	

/*
 * kern_return_t
 * bootstrap_check_in(port_t bootstrap_port,
 *	 name_t service_name,
 *	 port_t *service_portp)
 *
 * Returns receive rights to service_port of service named by service_name.
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 *		Returns BOOTSTRAP_NOT_PRIVILEGED, if request directed to
 *			unprivileged bootstrap port.
 *		Returns BOOTSTRAP_UNKNOWN_SERVICE, if service does not exist.
 *		Returns BOOTSTRAP_SERVICE_NOT_DECLARED, if service not declared
 *			in /etc/bootstrap.conf.
 *		Returns BOOTSTRAP_SERVICE_ACTIVE, if service has already been
 *			registered or checked-in.
 */
kern_return_t
x_bootstrap_check_in(
	port_t		bootstrap_port,
	name_t		service_name,
	port_all_t	*service_portp)
{
	kern_return_t result;
	service_t *servicep;
	server_t *serverp;
	bootstrap_t *bootstrap;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);
	serverp = lookup_server_by_port(bootstrap_port);
	if (serverp == NULL) {
		debug("bootstrap_check_in service %s has no server",
			service_name);
		return BOOTSTRAP_NOT_PRIVILEGED;
	}
	servicep = lookup_service_by_name(bootstrap, service_name);
	if (servicep == NULL || servicep->port == PORT_NULL) {
		debug("bootstrap_check_in service %s unknown%s", service_name,
			forward_ok ? " forwarding" : "");
		result = BOOTSTRAP_UNKNOWN_SERVICE;
		goto forward;
	}
	if (servicep->server != NULL && servicep->server != serverp) {
		debug("bootstrap_check_in service %s not privileged",
			service_name);
		 return BOOTSTRAP_NOT_PRIVILEGED;
	}
	if (servicep->servicetype == SELF || !canReceive(servicep->port)) {
		ASSERT(servicep->isActive);
		debug("bootstrap_check_in service %s already active",
			service_name);
		return BOOTSTRAP_SERVICE_ACTIVE;
	}
	log("Checkin service %s", service_name);
	ASSERT(servicep->isActive == FALSE);
	servicep->isActive = TRUE;
	*service_portp = servicep->port;
	info("Check-in port %d for service %s\n",
	      servicep->port, servicep->name);
	return BOOTSTRAP_SUCCESS;
    forward:
	return forward_ok
	 ? bootstrap_check_in(inherited_bootstrap_port,
			     service_name,
			     service_portp)
	 : result;
}

/*
 * kern_return_t
 * bootstrap_register(port_t bootstrap_port,
 *	name_t service_name,
 *	port_t service_port)
 *
 * Registers send rights for the port service_port for the service named by
 * service_name.  Registering a declared service or registering a service for
 * which bootstrap has receive rights via a port backup notification is
 * allowed.
 * The previous service port will be deallocated.  Restarting services wishing
 * to resume service for previous clients must first attempt to checkin to the
 * service.
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 *		Returns BOOTSTRAP_NOT_PRIVILEGED, if request directed to
 *			unprivileged bootstrap port.
 *		Returns BOOTSTRAP_SERVICE_ACTIVE, if service has already been
 *			register or checked-in.
 */
kern_return_t
x_bootstrap_register(
	port_t	bootstrap_port,
	name_t	service_name,
	port_t	service_port)
{
	service_t *servicep;
	server_t *serverp;
	bootstrap_t *bootstrap;
	port_t old_port;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);
	ASSERT(canSend(service_port));
	debug("Register attempt for service %s port %d",
	      service_name, service_port);

	/*
	 * If this bootstrap port is for a server, or it's an unprivileged
	 * bootstrap can't register the port.
	 */
	serverp = lookup_server_by_port(bootstrap_port);
	servicep = lookup_service_by_name(bootstrap, service_name);
	if (servicep && servicep->server && servicep->server != serverp)
		return BOOTSTRAP_NOT_PRIVILEGED;

	if (serverp)
		bootstrap_port = bootstrap->bootstrap_port;
	else if (bootstrap_port != bootstrap->bootstrap_port)
		return BOOTSTRAP_NOT_PRIVILEGED;

	if (servicep == NULL || servicep->bootstrap != bootstrap) {
		servicep = new_service(bootstrap,
				       service_name,
				       service_port,
				       ACTIVE,
				       REGISTERED,
				       NULL_SERVER,
				       NO_MACH_PORT);
		log("Registered new service %s", service_name);
	} else {
		if (servicep->isActive) {
			debug("Register: service %s already active, port %d",
		 	      servicep->name, servicep->port);
			ASSERT(!canReceive(servicep->port));
			return BOOTSTRAP_SERVICE_ACTIVE;
		}
		old_port = servicep->port;
		servicep->port = service_port;
		msg_destroy_port(old_port);
		servicep->isActive = TRUE;
		log("Re-registered inactive service %s", service_name);
	}
	info("Registering port %d for service %s\n",
	      servicep->port,
	      servicep->name);
	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_look_up(port_t bootstrap_port,
 *	name_t service_name,
 *	port_t *service_portp)
 *
 * Returns send rights for the service port of the service named by
 * service_name in *service_portp.  Service is not guaranteed to be active.
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 *		Returns BOOTSTRAP_UNKNOWN_SERVICE, if service does not exist.
 */
kern_return_t
x_bootstrap_look_up(
	port_t	bootstrap_port,
	name_t	service_name,
	port_t	*service_portp)
{
	service_t *servicep;
	bootstrap_t *bootstrap;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);
	servicep = lookup_service_by_name(bootstrap, service_name);
	if (servicep == NULL || servicep->port == PORT_NULL) {
		if (forward_ok) {
#if	DEBUG
			debug("bootstrap_look_up service %s forwarding",
				service_name);
#endif	DEBUG
			return bootstrap_look_up(inherited_bootstrap_port,
						service_name,
						service_portp);
		} else {
#if	DEBUG
			debug("bootstrap_look_up service %s unknown",
				service_name);
#endif	DEBUG
			return BOOTSTRAP_UNKNOWN_SERVICE;
		}
	}
	if (!canSend(servicep->port)) {
		error("Mysterious loss of send rights on port %d, "
		      "deleting service %s",
		      servicep->port,
		      servicep->name);
		delete_service(servicep);
		return BOOTSTRAP_UNKNOWN_SERVICE;
	}
	*service_portp = servicep->port;
#if	DEBUG
	debug("Lookup returns port %d for service %s\n",
	      servicep->port,
	      servicep->name);
#endif	DEBUG
	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_look_up_array(port_t bootstrap_port,
 *	name_array_t	service_names,
 *	int		service_names_cnt,
 *	port_array_t	*service_ports,
 *	int		*service_ports_cnt,
 *	boolean_t	*all_services_known)
 *
 * Returns port send rights in corresponding entries of the array service_ports
 * for all services named in the array service_names.  Service_ports_cnt is
 * returned and will always equal service_names_cnt (assuming service_names_cnt
 * is greater than or equal to zero).
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 *		Returns BOOTSTRAP_NO_MEMORY, if server couldn't obtain memory
 *			for response.
 *		Unknown service names have the corresponding service
 *			port set to PORT_NULL.
 *		If all services are known, all_services_known is true on
 *			return,
 *		if any service is unknown, it's false.
 */
kern_return_t
x_bootstrap_look_up_array(
	port_t		bootstrap_port,
	name_array_t	service_names,
	unsigned int	service_names_cnt,
	port_array_t	*service_portsp,
	unsigned int	*service_ports_cnt,
	boolean_t	*all_services_known)
{
	unsigned int i;
	static port_t service_ports[BOOTSTRAP_MAX_LOOKUP_COUNT];
	
	if (service_names_cnt > BOOTSTRAP_MAX_LOOKUP_COUNT)
		return BOOTSTRAP_BAD_COUNT;
	*service_ports_cnt = service_names_cnt;
	*all_services_known = TRUE;
	for (i = 0; i < service_names_cnt; i++) {
		if (   x_bootstrap_look_up(bootstrap_port,
					  service_names[i],
					  &service_ports[i])
		    != BOOTSTRAP_SUCCESS)
		{
			*all_services_known = FALSE;
			service_ports[i] = PORT_NULL;
		}
	}
#if	DEBUG
	debug("bootstrap_look_up_array returns %d ports", service_names_cnt);
#endif	DEBUG
	*service_portsp = service_ports;
	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_get_unpriv_port(port_t bootstrap_port, port_t *unpriv_port)
 *
 * Returns an unprivileged service port for bootstrap.  Unprivileged ports
 * disallow bootstrap_check_in and bootstrap_register requests.
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 */
kern_return_t
x_bootstrap_get_unpriv_port(port_t bootstrap_port, port_t *unpriv_port)
{
	bootstrap_t *bootstrap;
	
	bootstrap = lookup_bootstrap_by_port(bootstrap_port);
	ASSERT(canSend(bootstrap->unpriv_port));
	*unpriv_port = bootstrap->unpriv_port;
	debug("bootstrap_get_unpriv_port returns port %d\n",
		bootstrap->unpriv_port);
	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_status(port_t bootstrap_port,
 *	name_t service_name,
 *	boolean_t *service_active);
 *
 * Returns: service_active is true if service is available.
 *			
 * Errors:	Returns appropriate kernel errors on rpc failure.
 *		Returns BOOTSTRAP_UNKNOWN_SERVICE, if service does not exist.
 */
kern_return_t
x_bootstrap_status(
	port_t		bootstrap_port,
	name_t		service_name,
	boolean_t	*service_active)
{
	service_t *servicep;
	bootstrap_t *bootstrap;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);
	servicep = lookup_service_by_name(bootstrap, service_name);
	if (servicep == NULL) {
		if (forward_ok) {
			debug("bootstrap_status forwarding status, server %s",
				service_name);
			return bootstrap_status(inherited_bootstrap_port,
						service_name,
						service_active);
		} else {
#if	DEBUG
			debug("bootstrap_status service %s unknown",
				service_name);
#endif	DEBUG
			return BOOTSTRAP_UNKNOWN_SERVICE;
		}
	}
	*service_active = servicep->isActive;
#if	DEBUG
	debug("bootstrap_status server %s %sactive", service_name,
		servicep->isActive ? "" : "in");
#endif	DEBUG
	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_info(port_t bootstrap_port,
 *	name_array_t *service_names,
 *	int *service_names_cnt,
 *	name_array_t *server_names,
 *	int *server_names_cnt,
 *	bool_array_t *service_actives,
 *	int *service_active_cnt);
 *
 * Returns bootstrap status for all known services.
 *			
 * Errors:	Returns appropriate kernel errors on rpc failure.
 */
kern_return_t
x_bootstrap_info(
	port_t		bootstrap_port,
	name_array_t	*service_namesp,
	unsigned int	*service_names_cnt,
	name_array_t	*server_namesp,
	unsigned int	*server_names_cnt,
	bool_array_t	*service_activesp,
	unsigned int	*service_actives_cnt)
{
	kern_return_t result;
	unsigned int i, cnt;
	service_t *servicep;
	server_t *serverp;
	bootstrap_t *bootstrap;
	name_array_t service_names;
	name_array_t server_names;
	bool_array_t service_actives;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);

	for (   cnt = i = 0, servicep = services.next
	     ; i < nservices
	     ; servicep = servicep->next, i++)
	{
	    if (lookup_service_by_name(bootstrap, servicep->name) == servicep)
	    {
	    	cnt++;
	    }
	}
	result = vm_allocate(task_self(),
			     (vm_address_t *)&service_names,
			     cnt * sizeof(service_names[0]),
			     ANYWHERE);
	if (result != KERN_SUCCESS)
		return BOOTSTRAP_NO_MEMORY;

	result = vm_allocate(task_self(),
			     (vm_address_t *)&server_names,
			     cnt * sizeof(server_names[0]),
			     ANYWHERE);
	if (result != KERN_SUCCESS) {
		(void)vm_deallocate(task_self(),
				    (vm_address_t)service_names,
				    cnt * sizeof(service_names[0]));
		return BOOTSTRAP_NO_MEMORY;
	}
	result = vm_allocate(task_self(),
			     (vm_address_t *)&service_actives,
			     cnt * sizeof(service_actives[0]),
			     ANYWHERE);
	if (result != KERN_SUCCESS) {
		(void)vm_deallocate(task_self(),
				    (vm_address_t)service_names,
				    cnt * sizeof(service_names[0]));
		(void)vm_deallocate(task_self(),
				    (vm_address_t)server_names,
				    cnt * sizeof(server_names[0]));
		return BOOTSTRAP_NO_MEMORY;
	}

	for (  i = 0, servicep = services.next
	     ; i < nservices
	     ; servicep = servicep->next)
	{
	    if (   lookup_service_by_name(bootstrap, servicep->name)
		!= servicep)
		continue;
	    strncpy(service_names[i],
		    servicep->name,
		    sizeof(service_names[0]));
	    service_names[i][sizeof(service_names[0]) - 1] = '\0';
	    if (servicep->server) {
		    serverp = servicep->server;
		    strncpy(server_names[i],
			    serverp->cmd,
			    sizeof(server_names[0]));
		    server_names[i][sizeof(server_names[0]) - 1] = '\0';
		    debug("bootstrap info service %s server %s %sactive",
			servicep->name,
			serverp->cmd, servicep->isActive ? "" : "in"); 
	    } else {
		    server_names[i][0] = '\0';
		    debug("bootstrap info service %s %sactive",
			servicep->name, servicep->isActive ? "" : "in"); 
	    }
	    service_actives[i] = servicep->isActive;
	    i++;
	}
	*service_namesp = service_names;
	*server_namesp = server_names;
	*service_activesp = service_actives;
	*service_names_cnt = *server_names_cnt =
		*service_actives_cnt = cnt;

	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_subset(port_t bootstrap_port,
 *		    port_t requestor_port,
 *		    port_t *subset_port);
 *
 * Returns a new port to use as a bootstrap port.  This port behaves
 * exactly like the previous bootstrap_port, except that ports dynamically
 * registered via bootstrap_register() are available only to users of this
 * specific subset_port.  Lookups on the subset_port will return ports
 * registered with this port specifically, and ports registered with
 * ancestors of this subset_port.  Duplications of services already
 * registered with an ancestor port may be registered with the subset port
 * are allowed.  Services already advertised may then be effectively removed
 * by registering PORT_NULL for the service.
 * When it is detected that the requestor_port is destroied the subset
 * port and all services advertized by it are destroied as well.
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 */
kern_return_t
x_bootstrap_subset(
	port_t	bootstrap_port,
	port_t	requestor_port,
	port_t	*subset_port)
{
	kern_return_t result;
	bootstrap_t *bootstrap;
	bootstrap_t *subset;
	port_name_t new_bootstrap_port;
	port_name_t new_unpriv_port;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);

	result = port_allocate(task_self(), &new_bootstrap_port);
	if (result != KERN_SUCCESS)
		kern_fatal(result, "port_allocate");

	result = port_set_add(task_self(), bootstrap_port_set,
			      new_bootstrap_port);
	if (result != KERN_SUCCESS)
		kern_fatal(result, "port_set_add");

	result = port_allocate(task_self(), &new_unpriv_port);
	if (result != KERN_SUCCESS)
		kern_fatal(result, "port_allocate");

	result = port_set_add(task_self(), bootstrap_port_set,
			      new_unpriv_port);
	if (result != KERN_SUCCESS)
		kern_fatal(result, "port_set_add");

	subset = new_bootstrap(bootstrap, new_bootstrap_port, new_unpriv_port,
				requestor_port);
	*subset_port = new_bootstrap_port;
	info("bootstrap_subset new bootstrap %d unpriv %d",
		new_bootstrap_port, new_unpriv_port);
	return BOOTSTRAP_SUCCESS;
}

/*
 * kern_return_t
 * bootstrap_create_service(port_t bootstrap_port,
 *		      name_t service_name,
 *		      port_t *service_port)
 *
 * Creates a service named "service_name" and returns send rights to that
 * port in "service_port."  The port may later be checked in as if this
 * port were configured in the bootstrap configuration file.
 *
 * Errors:	Returns appropriate kernel errors on rpc failure.
 *		Returns BOOTSTRAP_NOT_PRIVILEGED, if request directed to
 *			unprivileged bootstrap port.
 *		Returns BOOTSTRAP_NAME_IN_USE, if service already exists.
 */
kern_return_t
x_bootstrap_create_service(
	port_t	bootstrap_port,
	name_t	service_name,
	port_t	*service_port)
{
	service_t *servicep;
	bootstrap_t *bootstrap;
	kern_return_t result;
	port_t previous;

	bootstrap = lookup_bootstrap_by_port(bootstrap_port);
	ASSERT(bootstrap);
	debug("Service creation attempt for service %s", service_name);

	/*
	 * If this bootstrap port is for  an unprivileged
	 * bootstrap can't create the service.
	 */
	if (bootstrap_port == bootstrap->unpriv_port)
		return BOOTSTRAP_NOT_PRIVILEGED;

	servicep = lookup_service_by_name(bootstrap, service_name);
	if (servicep) {
		debug("Service creation attempt for service %s failed, "
			"service already exists", service_name);
		return BOOTSTRAP_NAME_IN_USE;
	}

	result = port_allocate(task_self(), service_port);
	if (result != KERN_SUCCESS)
		kern_fatal(result, "port_allocate");

	result = port_set_backup(task_self(), *service_port, backup_port,
				 &previous);
	if (result != KERN_SUCCESS)
		kern_fatal(result, "port_set_backup");

	info("Declared port %d for service %s", *service_port,
		service_name);

	servicep = new_service(bootstrap,
				service_name,
				*service_port,
				!ACTIVE,
				DECLARED,
				NULL_SERVER,
				NO_MACH_PORT);

	log("Created new service %s", service_name);

	return BOOTSTRAP_SUCCESS;
}
