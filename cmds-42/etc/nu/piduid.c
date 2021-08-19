#include "piduid.h"

void *listGroups()
{
    ni_entrylist groupEntry;
    ni_status whatHappened;
    ni_namelist *referent;
    int index, groupcount = 0;
    
    NI_INIT (&groupEntry);
    whatHappened = ni_list(niHandle, &groupDir, "name", &groupEntry);
    if (whatHappened != NI_OK) {
	printf ("Unable to read the /groups directory.\n");
	ni_entrylist_free(&groupEntry);
	return;
    }
    for (index = 0; index < groupEntry.ni_entrylist_len; index ++) {
	referent = groupEntry.ni_entrylist_val[index].names;
	if ((referent != NULL) && (referent->ni_namelist_len != 0))
	    if (groupcount == 0)
		printf ("          %s", referent->ni_namelist_val[0]);
	    else
		printf("  %s", referent->ni_namelist_val[0]);
	    groupcount++;
	    if (groupcount == 4 && index != groupEntry.ni_entrylist_len - 1) {
		printf ("\n");
		groupcount = 0;
	    }
    }
    printf ("\n");
    ni_entrylist_free(&groupEntry);
    return FALSE;
}

int findUserInHmmm(char *userName, char *dirName, char *hmmmm, ni_id whatID)
{
    ni_entrylist hmmmEntry;
    ni_status  whatHappened;
    int where, index;
    ni_namelist *referent;

    NI_INIT (&hmmmEntry);
    whatHappened = ni_list(niHandle, &whatID, hmmmm, &hmmmEntry);
    if (whatHappened != NI_OK) {
	printf ("Unable to read %s directory.\nUnable to tell if user appears in any %s.\n", 
	    dirName, dirName);
	ni_entrylist_free(&hmmmEntry);
	return FALSE;
    }
    for (index = 0; index < hmmmEntry.ni_entrylist_len; index ++) {
	referent = hmmmEntry.ni_entrylist_val[index].names;
	if (referent != NULL)
	    where = ni_namelist_match (*referent, userName);
	if (where != NI_INDEX_NULL) /* matched */
	    return TRUE;
	else;
    }
    ni_entrylist_free(&hmmmEntry);
    return FALSE;
}

int getMaxUid()
{
    ni_entrylist uidEntries;
    ni_status whatHappened;
    int index, innerIndex;
    int maxValue = 0, currentValue;
    ni_namelist *referent;
    
    NI_INIT(&uidEntries);
    whatHappened = ni_list(niHandle, &userDir, "uid", &uidEntries);
    if (whatHappened != NI_OK) {
	printf ("Failure attempting to get maximum uid.  Unable to get uid entrylist from users directory.");
	exit (ERROR);
    }
    for (index = 0; index < uidEntries.ni_entrylist_len; index ++) {
	if (referent = uidEntries.ni_entrylist_val[index].names) /* user without uid?!?! */
	    for (innerIndex = 0; innerIndex < referent->ni_namelist_len; innerIndex++) {
		currentValue = atoi(referent->ni_namelist_val[innerIndex]);
		if (maxValue < currentValue)
		    maxValue = currentValue;
	    }
    }
    maxValue++;
    return maxValue;
}


struct passwd * mygetpwuid(int uid)
    /* getpwuid is a replacement for the system call, this version only matches things out of netinfo */
{
    char uidBuf[7];
    ni_entrylist uidEntries;
    ni_proplist uidProp;
    ni_property *referent;
    ni_status whatHappened;
    int index, innerIndex;
    ni_index whereMatched;
    register struct passwd *pw;
    ni_id tempID;
    
    sprintf(uidBuf, "%d", uid);
    NI_INIT(&uidEntries);
    NI_INIT(&uidProp);
    pw = NULL;
    retry:
    whatHappened = ni_list(niHandle, &userDir, "uid", &uidEntries);
    for (index = 0; index < uidEntries.ni_entrylist_len; index ++) {
        if (uidEntries.ni_entrylist_val[index].names == NULL) continue;
	whereMatched = ni_namelist_match (*uidEntries.ni_entrylist_val[index].names, uidBuf);
	if (whereMatched != NI_INDEX_NULL) {/* found the entry we's looking for */
	    tempID.nii_object = uidEntries.ni_entrylist_val[index].id;
	    whatHappened = ni_self(niHandle, &tempID);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&uidEntries);
		goto retry;
	    }
	    else;
	    whatHappened = ni_read (niHandle, &tempID, &uidProp);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&uidEntries);
		ni_proplist_free(&uidProp);
		goto retry;
	    }
	    else;
	    pw = (struct passwd *)malloc(sizeof(struct passwd));
	    pw->pw_gid = -2;
	    pw->pw_uid = -2;
	    pw->pw_dir = "";
	    pw->pw_name = "";
	    pw->pw_passwd = "";
	    pw->pw_gecos = "";
	    pw->pw_quota = 0;
	    for(innerIndex = 0; innerIndex < uidProp.ni_proplist_len; innerIndex++) {
		referent = &uidProp.ni_proplist_val[innerIndex];
		/* set up default values before scan */
		switch (referent->nip_name[0]) {
		    case 'g':
			ni_name_match(referent->nip_name, "gid");
			if (referent->nip_val.ni_namelist_len > 0);
			    pw->pw_gid = atoi(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'h':
			ni_name_match(referent->nip_name, "home");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_dir = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'n':
			ni_name_match(referent->nip_name, "name");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_name = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'p':
			ni_name_match(referent->nip_name, "passwd");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_passwd = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'r':
			ni_name_match(referent->nip_name, "realname");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_gecos = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 's':
			ni_name_match(referent->nip_name, "shell");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_shell = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'u':
			ni_name_match(referent->nip_name, "uid");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_uid = atoi(referent->nip_val.ni_namelist_val[0]);
		    default: ;
		}
	    }
	}
    }
    ni_proplist_free(&uidProp);
    ni_entrylist_free(&uidEntries);
    return pw;
}

struct passwd * mygetpwnam(char *nam)
{
    ni_entrylist namEntries;
    ni_proplist namProp;
    ni_property *referent;
    ni_status whatHappened;
    int index, innerIndex;
    ni_index whereMatched;
    register struct passwd *pw;
    ni_id tempID;
    
    NI_INIT(&namEntries);
    NI_INIT(&namProp);
    pw = NULL;
    retry:
    whatHappened = ni_list(niHandle, &userDir, "name", &namEntries);
    for (index = 0; index < namEntries.ni_entrylist_len; index ++) {
        if (namEntries.ni_entrylist_val[index].names == NULL) continue;
	whereMatched = ni_namelist_match (*namEntries.ni_entrylist_val[index].names, nam);
	if (whereMatched != NI_INDEX_NULL) {/* found the entry we's looking for */
	    tempID.nii_object = namEntries.ni_entrylist_val[index].id;
	    whatHappened = ni_self(niHandle, &tempID);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&namEntries);
		goto retry;
	    }
	    else;
	    whatHappened = ni_read (niHandle, &tempID, &namProp);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&namEntries);
		ni_proplist_free(&namProp);
		goto retry;
	    }
	    else;
	    pw = (struct passwd *)malloc(sizeof(struct passwd));
	    pw->pw_gid = -2;
	    pw->pw_uid = -2;
	    pw->pw_dir = "";
	    pw->pw_name = "";
	    pw->pw_passwd = "";
	    pw->pw_gecos = "";
	    pw->pw_quota = 0;
	    for(innerIndex = 0; innerIndex < namProp.ni_proplist_len; innerIndex++) {
		referent = &namProp.ni_proplist_val[innerIndex];
		/* set up default values before scan */
		switch (referent->nip_name[0]) {
		    case 'g':
			ni_name_match(referent->nip_name, "gid");
			if (referent->nip_val.ni_namelist_len > 0);
			    pw->pw_gid = atoi(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'h':
			ni_name_match(referent->nip_name, "home");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_dir = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'n':
			ni_name_match(referent->nip_name, "name");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_name = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'p':
			ni_name_match(referent->nip_name, "passwd");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_passwd = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'r':
			ni_name_match(referent->nip_name, "realname");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_gecos = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 's':
			ni_name_match(referent->nip_name, "shell");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_shell = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'u':
			ni_name_match(referent->nip_name, "uid");
			if (referent->nip_val.ni_namelist_len > 0)
			    pw->pw_uid = atoi(referent->nip_val.ni_namelist_val[0]);
			break;
		    default: ;
		}
	    }
	}
    }
    ni_proplist_free(&namProp);
    ni_entrylist_free(&namEntries);
    return pw;
}

struct group * mygetgrgid(int gid)
{
    char  gidBuf[7];
    ni_entrylist gidEntries;
    ni_proplist gidProp;
    ni_property *referent;
    ni_status whatHappened;
    int index, innerIndex, eachgid;
    ni_index whereMatched;
    register struct group *grp;
    ni_id tempID;
    
    sprintf(gidBuf, "%d", gid);
    NI_INIT(&gidEntries);
    NI_INIT(&gidProp);
    grp = NULL;
    retry:
    whatHappened = ni_list(niHandle, &groupDir, "gid", &gidEntries);
    for (index = 0; index < gidEntries.ni_entrylist_len; index ++) {
        if (gidEntries.ni_entrylist_val[index].names == NULL) continue; /* they shouldn't do that! */
	whereMatched = ni_namelist_match (*gidEntries.ni_entrylist_val[index].names, gidBuf);
	if (whereMatched != NI_INDEX_NULL) {/* found the entry we's looking for */
	    tempID.nii_object = gidEntries.ni_entrylist_val[index].id;
	    whatHappened = ni_self(niHandle, &tempID);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&gidEntries);
		goto retry;
	    }
	    else;
	    whatHappened = ni_read (niHandle, &tempID, &gidProp);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&gidEntries);
		ni_proplist_free(&gidProp);
		goto retry;
	    }
	    else;
	    grp = (struct group *)malloc(sizeof(struct group));
	    grp->gr_gid = -2;
	    grp->gr_mem = NULL;
	    grp->gr_name = "";
	    grp->gr_passwd = "*";
	    for(innerIndex = 0; innerIndex < gidProp.ni_proplist_len; innerIndex++) {
		referent = &gidProp.ni_proplist_val[innerIndex];
		/* set up default values before scan */
		switch (referent->nip_name[0]) {
		    case 'g':
			ni_name_match(referent->nip_name, "gid");
			if (referent->nip_val.ni_namelist_len > 0)
			    grp->gr_gid = atoi(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'n':
			ni_name_match(referent->nip_name, "name");
			if (referent->nip_val.ni_namelist_len > 0)
			    grp->gr_name = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'p':
			ni_name_match(referent->nip_name, "passwd");
			if (referent->nip_val.ni_namelist_len > 0)
			    grp->gr_passwd = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'u':
			ni_name_match(referent->nip_name, "users");
			if (referent->nip_val.ni_namelist_len > 0){
			    grp->gr_mem = (char **)malloc(referent->nip_val.ni_namelist_len * sizeof(char *) + 1);
			    grp->gr_mem[referent->nip_val.ni_namelist_len] = NULL;
			    for (eachgid = 0; eachgid < referent->nip_val.ni_namelist_len; eachgid++)
				grp->gr_mem[eachgid] = ni_name_dup
				    (referent->nip_val.ni_namelist_val[eachgid]);
			    }
			break;
		    default: ;
		}
	    }
	}
    }
    ni_proplist_free(&gidProp);
    ni_entrylist_free(&gidEntries);
    return grp;
}

struct group * mygetgrnam(char *nam)
{
    ni_entrylist namEntries;
    ni_proplist namProp;
    ni_property *referent;
    ni_status whatHappened;
    int index, innerIndex, eachname;
    ni_index whereMatched;
    register struct group *grp;
    ni_id tempID;
    
    NI_INIT(&namEntries);
    NI_INIT(&namProp);
    grp = NULL;
    retry:
    whatHappened = ni_list(niHandle, &groupDir, "name", &namEntries);
    for (index = 0; index < namEntries.ni_entrylist_len; index ++) {
        if (namEntries.ni_entrylist_val[index].names == NULL) continue;
	whereMatched = ni_namelist_match (*namEntries.ni_entrylist_val[index].names, nam);
	if (whereMatched != NI_INDEX_NULL) {/* found the entry we's looking for */
	    tempID.nii_object = namEntries.ni_entrylist_val[index].id;
	    whatHappened = ni_self(niHandle, &tempID);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&namEntries);
		goto retry;
	    }
	    else;
	    whatHappened = ni_read (niHandle, &tempID, &namProp);
	    if (whatHappened != NI_OK) { /* try again, this can only fail a limited number of times */
		ni_entrylist_free(&namEntries);
		ni_proplist_free(&namProp);
		goto retry;
	    }
	    else;
	    grp = (struct group *)malloc(sizeof(struct group));
	    grp->gr_gid = -2;
	    grp->gr_mem = NULL;
	    grp->gr_name = "";
	    grp->gr_passwd = "*";
	    for(innerIndex = 0; innerIndex < namProp.ni_proplist_len; innerIndex++) {
		referent = &namProp.ni_proplist_val[innerIndex];
		/* set up default values before scan */
		switch (referent->nip_name[0]) {
		    case 'g':
			ni_name_match(referent->nip_name, "gid");
			if (referent->nip_val.ni_namelist_len > 0)
			    grp->gr_gid = atoi(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'n':
			ni_name_match(referent->nip_name, "name");
			if (referent->nip_val.ni_namelist_len > 0)
			    grp->gr_name = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'p':
			ni_name_match(referent->nip_name, "passwd");
			if (referent->nip_val.ni_namelist_len > 0)
			    grp->gr_passwd = ni_name_dup(referent->nip_val.ni_namelist_val[0]);
			break;
		    case 'u':
			ni_name_match(referent->nip_name, "users");
			if (referent->nip_val.ni_namelist_len > 0){
			    grp->gr_mem = (char **)malloc(referent->nip_val.ni_namelist_len * sizeof(char *) + 1);
			    grp->gr_mem[referent->nip_val.ni_namelist_len] = NULL;
			    for (eachname = 0; eachname < referent->nip_val.ni_namelist_len; eachname ++)
				grp->gr_mem[eachname] =
				ni_name_dup(referent->nip_val.ni_namelist_val[eachname]);
			    }
			break;
		    default: ;
		}
	    }
	}
    }
    ni_proplist_free(&namProp);
    ni_entrylist_free(&namEntries);
    return grp;
}


