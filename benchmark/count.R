
png(filename="FM_count_200MB_Q50000.png",width=600,height=450);

m <- c(5,10,15,20,25,30,40,50);

wsj200_t <- c(3.004277,6.868446,10.616327,14.343132,18.771059,21.767828,29.197590,36.649755);
src200_t <- c(3.625269,8.185533,12.621822,17.045359,21.526136,25.909429,34.632518,43.540632);
xml200_t <- c(3.431548,7.866460,12.295890,16.740950,21.170851,25.571471,34.338939,43.126175);
dna200_t <- c(1.083739,2.684546,4.256207,5.807457,7.346458,8.881236,11.973110,15.042318);
pro200_t <- c(1.942342,4.412161,6.830325,9.256430,11.670832,14.086307,18.914170,23.748409);

plot(m,wsj200_t,type="b",ylim=c(0,45),xlab="pattern length",ylab="time [sec]",col="red",lwd=3,pch=1,main="FM-Index count() - 200MB - 50000 queries");
lines(m,src200_t,type="b",col="green",lwd=3,pch=2);
lines(m,xml200_t,type="b",col="blue",lwd=3,pch=3);
lines(m,dna200_t,type="b",col="orange",lwd=3,pch=4);
lines(m,pro200_t,type="b",col="black",lwd=3,pch=5);
legend("topleft",c("wsj","src","xml","dna","protein"),col=c("red","green","blue","orange","black"),bty="n",lwd=3,pch=c(1:5));

dev.off();