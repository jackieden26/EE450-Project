all: serverA.c serverB.c aws.c client.c
			gcc -o serverA serverA.c
			gcc -o serverB serverB.c
			gcc -o aws aws.c
			gcc -o client client.c

.PHONY: serverA serverB aws

serverA:
			@./serverA

serverB:
			@./serverB

aws:
			@./aws

clean:
			rm -f serverA serverB aws client
