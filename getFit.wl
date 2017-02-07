faradayData=Import["/home/pi/RbData/2016-08-11/FDayScan2016-08-11_122000.dat","Data"]
(*Remove lines from the file. This one gets rid of comments.*)
(* I need to first extract the magnetic field and probe offset information *)
faradayData=Delete[faradayData,{{1},{2},{3},{4},{5},{6},{7},{8},{9},{10}}]
(*Remove columns from the file. This one gets rid of unneeded fourier coefficients*)
faradayData=Drop[faradayData,None,{2}]
faradayData=Drop[faradayData,None,{2}]
faradayData=Drop[faradayData,None,{2}]
faradayData=Drop[faradayData,None,{2}]
faradayData=Drop[faradayData,None,{2}]
faradayData=Drop[faradayData,None,{3}]
(*Remove datapoints that I'm not interested in.*)
(*faradayData=Delete[faradayData,{{1}}]*)
(*l=2.8 (*cm*)*)
c=2.9979*^10 (*cm/s*)
re=2.8179*^-13 (*cm*)
fge=0.34231 (*UNITLESS*)
k=4/3 (* UNITLESS *)
Mb=9.2740*^-24 (* J/T *)
BdotL=2.08*^-2 (* T *)
h=6.6261*^-34 (* J*s *)
vo=3.77107*^14 (* 1/s *) 
pi=3.14159265 (* DIMENSIONLESS *)
aoutConv=-.0222
aoutIntercept=19.002
const=c*re*fge*k*Mb*BdotL/(4*pi*h*vo)
model=a+b*const*(vo+(aoutConv*detune+aoutIntercept)*1*^9)/((aoutConv*detune+aoutIntercept)*1*^9)^2+d*(vo+(aoutConv*detune+aoutIntercept)*1*^9)^5/((aoutConv*detune+aoutIntercept)*1*^9)^4
modelResult=NonlinearModelFit[faradayData,model,{a,b,d},detune]
modelResult["BestFitParameters"]>>fitParams.txt
modelResult["ParameterErrors"]>>>fitParams.txt
