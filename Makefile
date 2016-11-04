SMH_SOURCE  = smh_*.c
SMH_HEADER  = smh_*.h
SMH_FLAGS   = -DDRIVER_X
SMH_LIBS    = -lX11 -lrt -lm
SMH_DEBUG   = -g3 -Wall -Wextra -DDEBUG
SMH_RELEASE = -Ofast

.PHONY:	clean debug

debug:	smh
release:	smh.release
smh:	${SMH_SOURCE} ${SMH_HEADER}
	gcc ${SMH_LIBS} ${SMH_FLAGS} ${SMH_DEBUG} -o smh ${SMH_SOURCE}
smh.release:	${SMH_SOURCE} ${SMH_HEADER}
	gcc ${SMH_LIBS} ${SMH_FLAGS} ${SMH_RELEASE} -o smh.release ${SMH_SOURCE}
	strip smh.release
	@echo $( which upx && upx -9 smh.release 2>&1 > /dev/null ) > /dev/null 
clean:
	rm -rf smh
