.PHONY: clone
#clone
clone:
	gcc namespace.c -o clone

.PHONY: setns
setns:
	gcc setns.c -o setns

.PHONY: unshare
unshare:
	gcc unshare.c -o unshare

.PHONY: fork
fork:
	gcc fork.c -o fork