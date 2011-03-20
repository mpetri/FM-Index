
png(filename="FM_locate_200MB_QL5.png",width=600,height=450);

m <- c(5000,10000,20000,40000,80000,160000,320000);

wsj200_t <- c(2.608162,5.365946,10.602519,21.442579,42.938683,85.137125,171.658761);
src200_t <- c(3.149645,6.255422,12.319497,24.334684,48.917122,97.516541,193.819999);
xml200_t <- c(3.174452,5.973081,11.696171,22.978747,46.128351,94.984783,182.533758);
dna200_t <- c(1.227305,2.330018,4.662057,9.053759,18.440359,36.530764,73.798636);
pro200_t <- c(1.640507,3.213040,6.306421,12.449543,25.456065,49.679867,99.471245);

plot(m,wsj200_t,type="b",ylim=c(0,200),xlab="number of occurrences",ylab="time [sec]",col="red",lwd=3,pch=1,main="FM-Index locate() - 200MB - pattern length 5");
lines(m,src200_t,type="b",col="green",lwd=3,pch=2);
lines(m,xml200_t,type="b",col="blue",lwd=3,pch=3);
lines(m,dna200_t,type="b",col="orange",lwd=3,pch=4);
lines(m,pro200_t,type="b",col="black",lwd=3,pch=5);
legend("topleft",c("wsj","src","xml","dna","protein"),col=c("red","green","blue","orange","black"),bty="n",lwd=3,pch=c(1:5));

dev.off();