# @(#)subsidiary.mc	0.8 88/10/17 NeXT
###########################################################
#
#	Sendmail configuration file for SUBSIDIARY NeXT machines.
#
#	You should install this file as
#	/etc/sendmail/sendmail.cf if your
#	machine is a subsidiary machine (that is, some other machine
#	in your domain is the main mail-relaying machine).  You should
#	not need to edit this file to customize it for your network
#	configuration.  If you do, then you probably want to use
#	sendmail.main.cf.
#
#	See the paper "Sendmail Installation and Administration Guide"
#	for more information on the format of this file.
#
#	@(#)subsidiary.mc 0.8 88/10/17 NeXT; from SMI 3.2/4.3 NFSSRC
#

# local UUCP connections -- not forwarded to mailhost
# The local UUCP connections are output by the uuname program.
FV|/usr/bin/uuname

# my official hostname
Dj$w.$m

# major relay mailer
DMether

# major relay host
DRmailhost
CRmailhost

include(nextbase.m4)

include(uucpm.m4)

include(zerobase.m4)

################################################
###  Machine dependent part of ruleset zero  ###
################################################

# resolve names we can handle locally
R<@$=V.uucp>:$+		$:$>9 $1			First clean up, then...
R<@$=V.uucp>:$+		$#uucp  $@$1 $:$2		@host.uucp:...
R$+<@$=V.uucp>		$#uucp  $@$2 $:$1		user@host.uucp
R<@$=V>:$+		$#uucp  $@$1 $:$2		@host.uucp:...
R$+<@$=V>		$#uucp  $@$2 $:$1		user@host.uucp

# non-local UUCP hosts get kicked upstairs
R$+<@$+.uucp>		$#$M  $@$R $:$2!$1

# optimize names of known ethernet hosts
R$*<@$+.LOCAL>$*	$#ether $@$2 $:$1<@$2>$3	user@host.here

# other non-local names will be kicked upstairs
R$+			$:$>9 $1			Clean up, keep <>
R$*<@$+>$*		$#$M    $@$R $:$1<@$2>$3	user@some.where
R$*@$*			$#$M    $@$R $:$1<@$2>		strangeness with @

# Local names with % are really not local!
R$+%$+			$@$>30$1@$2			turn % => @, retry

# everything else is a local name
R$+			$#local $:$1			local names
