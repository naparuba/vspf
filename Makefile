belial.o: belial.c belial.h
	gcc -c -Wall belial.c
time_work_asked_list.o: time_work_asked_list.c time_work_asked_list.h
	gcc -c -Wall time_work_asked_list.c
time_work_asked.o: time_work_asked.c time_work_asked.h
	gcc -c -Wall time_work_asked.c
auron.o: auron.c auron.h
	gcc -g -Wall -c auron.c 
wakka.o: wakka.c wakka.c
	gcc -c -Wall wakka.c
itachi.o: itachi.c itachi.h
	gcc -c -Wall itachi.c
rochel.o: rochel.c rochel.h
	gcc -c -Wall rochel.c
kira.o: kira.c kira.h
	gcc -c -Wall kira.c
yuna.o: yuna.h yuna.c
	gcc -c -Wall yuna.c
kimari.o: kimari.c kimari.h
	gcc -c -Wall kimari.c
tidus.o: tidus.c tidus.h
	gcc -c -Wall tidus.c
caract_list.o: caract_list.c caract_list.h
	gcc -c -Wall caract_list.c
messages.o: messages.c messages.h
	gcc -c -Wall messages.c 
ipq_list.o: ipq_list.c ipq_list.h
	gcc -c -Wall ipq_list.c
rule_list.o: rule_list.c rule_list.h
	gcc -c -Wall rule_list.c
rule.o: rule.c rule.h
	gcc -c -Wall rule.c
caract.o: caract.c caract.h
	gcc -c -Wall caract.c
print.o: print.c print.h
	gcc -c -Wall print.c
packet.o: packet.c packet.h
	gcc -c -Wall packet.c
guardians.o:packet.o print.o time_work_asked_list.o time_work_asked.o wakka.o auron.o caract.o rule.o rule_list.o messages.o yuna.o kimari.o tidus.o
	gcc -c guardians.c -Wall
all: packet.o print.o rochel.o itachi.o caract.o kira.o belial.o time_work_asked_list.o time_work_asked.o rule.o rule_list.o ipq_list.o messages.o guardians.o packet.o print.o time_work_asked_list.o time_work_asked.o wakka.o auron.o caract.o rule.o rule_list.o messages.o yuna.o kimari.o tidus.o
	gcc sin.c guardians.o print.o rochel.o itachi.o belial.o time_work_asked_list.o time_work_asked.o kira.o rule_list.o rule.o ipq_list.o  packet.o wakka.o auron.o caract.o messages.o yuna.o kimari.o tidus.o -o sin -Wall -lipq
ihm: messages.o rule.o print.o packet.o caract_list.o
	gcc `pkg-config --libs --cflags gtk+-2.0` -Wall base_ihm.c rule.o print.o packet.o messages.o caract_list.o -o base_ihm 
clean:
	rm -f sin *.o *~
clean-ihm:
	rm -f ihm *.o *~