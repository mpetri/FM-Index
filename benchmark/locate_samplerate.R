
png(filename="FM_locate_samplerate_50MB_QL6.png",width=600,height=450);

s <- c("8","16","32","64","128","256");

wsj50_t <- c(1.938084,4.147383,8.862773,18.415916,37.629284,76.178948,151.487517);
wsj50_s <- c(75,49,36,29,26,24,23);

plot(wsj50_s,wsj50_t,type="b",xlab="space [MB]",ylab="time [sec]",col="red",lwd=3,pch=1,main="FM-Index locate() - 50MB - 40000 occurences");
abline(v=50,lwd=3);
text(x=75,y=1.938084+9,"8");
text(x=49,y=4.147383+9,"16");
text(x=36,y=8.862773+9,"32");
text(x=29+1,y=18.415916+9,"64");
text(x=26-1.7,y=37.629284-1.2,"128");
text(x=24+2,y=76.178948+2,"256");
text(x=23+2,y=151.487517,"512");
text(x=53,y=120,"text size");

dev.off();