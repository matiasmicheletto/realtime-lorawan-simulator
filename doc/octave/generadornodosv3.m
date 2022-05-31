close all; clear all;

periods=[3600,3600,3600,ones(1,97)*3600];
H=1;
for i=1:length(periods),
  H=lcm(H,periods(i));
endfor
H
eds=6600;
lado=250;
R=250;
%calculo en función del lado la cantidad estimada de GW a colocar siguiendo 
%regla de raiz(3)xR (tomo R=250). Entre centros 430m. De alli surge entonces,
% techo(L/(2R))
%NroGWI=ceil(lado/(2*R));

nodosORIG=[(1:eds)',randi(lado,[eds,1])-lado/2,randi(lado,[eds,1])-lado/2,zeros(eds,1),periods(randi(100,eds,1))',zeros(eds,2)];
nodosORIG=[nodosORIG,H./nodosORIG(:,5)];

%Primer gw ubicado en -215,0

nodos=nodosORIG;
figure(1)
scatter(nodos(:,2), nodos(:,3), '.')
title("Posiciones")

nodos(:,2)=nodos(:,2);
nodos(:,3)=nodos(:,3);
nodos(:,4)=sqrt(nodos(:,2).^2+nodos(:,3).^2);

%nodos=[(1:eds)',nodos,sqrt(nodos(:,1).^2+nodos(:,2).^2),periods(randi(100,eds,1))',zeros(eds,2)];
%nodos=[nodos,H./nodos(:,5)];

for i=1:length(nodos),
  if (nodos(i,5)/100>32)
    nodos(i,7)=12;
  elseif (nodos(i,5)/100>16)
    nodos(i,7)=11;
  elseif (nodos(i,5)/100>8)
    nodos(i,7)=10;
  elseif (nodos(i,5)/100>4)
    nodos(i,7)=9;
   elseif (nodos(i,5)/100>2)
    nodos(i,7)=8;
  elseif (nodos(i,5)/100>1)
    nodos(i,7)=7;
  endif 
endfor  
%scatter(nodos(:,1), nodos(:,2), '.')
%title("Posiciones")

for i=1:length(nodos(:,1)),
  if (nodos(i,4)<62.5)
    nodos(i,6)=min(7,nodos(i,7));
  elseif (nodos(i,4)<125)% && nodos(i,5)==9000)    
    nodos(i,6)=min(8,nodos(i,7));
  elseif (nodos(i,4)<250)% && nodos(i,5)==9000)    
    nodos(i,6)=min(9,nodos(i,7));
  elseif (nodos(i,4)<500)% && nodos(i,5)==9000)    
    nodos(i,6)=min(10,nodos(i,7));
  elseif (nodos(i,4)<1000)% && nodos(i,5)==9000)    
    nodos(i,6)=min(11,nodos(i,7));
  elseif (nodos(i,4)<2000)% && nodos(i,5)==9000)    
    nodos(i,6)=min(12,nodos(i,7));
  endif
endfor

devices=sortrows(nodos,6);
MsgToT=sum(devices(:,8))
%devices=sortrows(devices,4)
F=find(devices(:,6)<100);
Fnodes=devices(1:length(F),:);
%Fnodes=sortrows(Fnodes,5);

if (length(F)<eds)
  NFnodes=devices(length(F(:,1))+1:eds,:);
else
  NFnodes=zeros(1,8);
end

MsgsT_P=sum(Fnodes(:,8))

MsgSF7=Fnodes(1:length(find(Fnodes(:,6)<8)),:);
%papa=length(find(Fnodes(:,6)>8))
if (length(find(Fnodes(:,6)>8))>0 && length(find(Fnodes(:,6)<9)>0))
  MsgSF8=Fnodes(length(find(Fnodes(:,6)<8))+1:length(find(Fnodes(:,6)<9)),:);
  if (length(find(Fnodes(:,6)>9))>0 && length(find(Fnodes(:,6)<10)>0))
    MsgSF9=Fnodes(length(find(Fnodes(:,6)<9))+1:length(find(Fnodes(:,6)<10)),:);
    if (length(find(Fnodes(:,6)>10))>0 && length(find(Fnodes(:,6)<11)>0))
      MsgSF10=Fnodes(length(find(Fnodes(:,6)<10))+1:length(find(Fnodes(:,6)<11)),:);
      if (length(find(Fnodes(:,6)>11))>0 && length(find(Fnodes(:,6)<12)>0))
        MsgSF11=Fnodes(length(find(Fnodes(:,6)<11))+1:length(find(Fnodes(:,6)>12)),:);
        if (length(find(Fnodes(:,6)>11))>0)
          MsgSF12=Fnodes(length(find(Fnodes(:,6)<12))+1:length(Fnodes(:,6)),:);
        end
      else
        MsgSF11=Fnodes(length(find(Fnodes(:,6)<11)+1):length(Fnodes(:,6)),:);
        MsgSF12=zeros(1,8);
       end
    else
      MsgSF10=Fnodes(length(find(Fnodes(:,6)<10)+1):length(Fnodes(:,6)),:);
      MsgSF11=zeros(1,8);
      MsgSF12=zeros(1,8);
    end
  else
    MsgSF9=Fnodes(length(find(Fnodes(:,6)<9)+1):length(Fnodes(:,6)),:);
    MsgSF10=zeros(1,8);
    MsgSF11=zeros(1,8);
    MsgSF12=zeros(1,8);
    end
else
  MsgSF8=Fnodes(length(find(Fnodes(:,6)<8))+1:length(Fnodes(:,6)),:);
  MsgSF9=zeros(1,8);
  MsgSF10=zeros(1,8);
  MsgSF11=zeros(1,8);
  MsgSF12=zeros(1,8);
end


% Factibilidad
%Acomodo los msgs por SF hasta saturar cada uno. Lo que queda lo sumo a los nodos no factibles.
final=0
while(final==0)
  if (sum(MsgSF7(:,8))<=H)
    if (sum(MsgSF8(:,8))<=H/2)
      if (sum(MsgSF9(:,8))<=H/4)
          if (sum(MsgSF10(:,8))<=H/8)
            if (sum(MsgSF11(:,8))<=H/16)
              if (sum(MsgSF12(:,8))<=H/32)
                 factibe=1;
                 final=1;
              else
                factible=0
                i=1;
                while (sum(MsgSF12(1:i,8))<=H/32)
                  i=i+1;
                end
                MsgSF12(i:length(MsgSF12),6)=MsgSF12(i:length(MsgSF12),6)+88;
                NFnodes=[MsgSF12(i:length(MsgSF12),:);NFnodes];
                MsgSF12=MsgSF12(1:i-1,:);
                final=1 
              end
            else
   %           if (sum(MsgSF12(:,8))<=112)
                i=1;
               while (sum(MsgSF11(1:i,8))<=H/16)
                  i=i+1;
                end
                MsgSF11(i:length(MsgSF11),6)=MsgSF11(i:length(MsgSF11),6)+1;
                MsgSF12=[MsgSF11(i:length(MsgSF11),:);MsgSF12];
                MsgSF11=MsgSF11(1:i-1,:);
    %          end
            end  
          else
  %          if (sum(MsgSF11(:,8))<=250)
              i=1;
              while (sum(MsgSF10(1:i,8))<=H/8)
                i=i+1;
              end
              MsgSF10(i:length(MsgSF10),6)=MsgSF10(i:length(MsgSF10),6)+1;
              MsgSF11=[MsgSF10(i:length(MsgSF10),:);MsgSF11];
              MsgSF10=MsgSF10(1:i-1,:);
   %         end
          end  
      else
 %       if (sum(MsgSF10(:,8))<=450)
          i=1;
          while (sum(MsgSF9(1:i,8))<=H/4)
            i=i+1;
          end
          MsgSF9(i:length(MsgSF9),6)=MsgSF9(i:length(MsgSF9),6)+1;
          MsgSF10=[MsgSF9(i:length(MsgSF9),:);MsgSF10];
          MsgSF9=MsgSF9(1:i-1,:);
 %       end
      end
    else
%       if (sum(MsgSF9(:,8))<=900)
          i=1;
          while (sum(MsgSF8(1:i,8))<=H/2)
            i=i+1;
          end
          MsgSF8(i:length(MsgSF8),6)=MsgSF8(i:length(MsgSF8),6)+1;
          MsgSF9=[MsgSF8(i:length(MsgSF8),:);MsgSF9];
          MsgSF8=MsgSF8(1:i-1,:);
 %       end
    end
  else
  %  if (sum(MsgSF8(:,8))<=1800)
      i=1;
      while (sum(MsgSF7(1:i,8))<=H)
        i=i+1;
      end
      MsgSF7(i:length(MsgSF7),6)=MsgSF7(i:length(MsgSF7),6)+1;
      MsgSF8=[MsgSF7(i:length(MsgSF7),:);MsgSF8];
      MsgSF7=MsgSF7(1:i-1,:);
  %  end  
  end
endwhile
%NFnodes=devices(length(F(:,1))+1:eds,:);
SF7=sum(MsgSF7(:,8))
SF8=sum(MsgSF8(:,8))
SF9=sum(MsgSF9(:,8))
SF10=sum(MsgSF10(:,8))
SF11=sum(MsgSF11(:,8))
SF12=sum(MsgSF12(:,8))
NF=sum(NFnodes(:,8))
%scatter(NFnodes(:,2),NFnodes(:,3),'.r')
%figure(1)
%scatter(NFnodes(:,2),NFnodes(:,3),'.r')
%hold
%scatter(MsgSF7(:,2),MsgSF7(:,3),'x b')
%scatter(MsgSF8(:,2),MsgSF8(:,3),'o r')
%scatter(MsgSF9(:,2),MsgSF9(:,3),'. k')
%scatter(MsgSF10(:,2),MsgSF10(:,3),'- y')
%scatter(MsgSF11(:,2),MsgSF11(:,3),'x m')
%scatter(MsgSF12(:,2),MsgSF12(:,3),'o c')
%grid
%legend('nodos','sf7','sf8','sf9','sf10','sf11','sf12')

%%%%%%%Calculo del segundo gw
if (NFnodes(1,1)>0)
nodos=NFnodes;
%nodos(:,2)=nodos(:,2)+430;
%nodos(:,4)=sqrt(nodos(:,3).^2+nodos(:,3).^2);
NFnodes(:,2)=NFnodes(:,2);
figure(2)
scatter(NFnodes(:,2), NFnodes(:,3), '.')
title("Posiciones No asignados primer gw")

nodos(:,2)=nodos(:,2);%+300;
nodos(:,3)=nodos(:,3);
nodos(:,4)=sqrt(nodos(:,2).^2+nodos(:,3).^2);

for i=1:length(nodos),
  if (nodos(i,5)/100>32)
    nodos(i,7)=12;
  elseif (nodos(i,5)/100>16)
    nodos(i,7)=11;
  elseif (nodos(i,5)/100>8)
    nodos(i,7)=10;
  elseif (nodos(i,5)/100>4)
    nodos(i,7)=9;
   elseif (nodos(i,5)/100>2)
    nodos(i,7)=8;
  elseif (nodos(i,5)/100>1)
    nodos(i,7)=7;
  endif 
endfor  


for i=1:length(nodos(:,1)),
  if (nodos(i,4)<62.5)
    nodos(i,6)=min(7,nodos(i,7));
  elseif (nodos(i,4)<125)% && nodos(i,5)==9000)    
    nodos(i,6)=min(8,nodos(i,7));
  elseif (nodos(i,4)<250)% && nodos(i,5)==9000)    
    nodos(i,6)=min(9,nodos(i,7));
  elseif (nodos(i,4)<500)% && nodos(i,5)==9000)    
    nodos(i,6)=min(10,nodos(i,7));
  elseif (nodos(i,4)<1000)% && nodos(i,5)==9000)    
    nodos(i,6)=min(11,nodos(i,7));
  elseif (nodos(i,4)<2000)% && nodos(i,5)==9000)    
    nodos(i,6)=min(12,nodos(i,7));
  endif
endfor
devices2=sortrows(nodos,6);
MsgToT2=sum(devices2(:,8))
%devices=sortrows(devices,4)
F2=find(devices2(:,6)<100);
Fnodes2=devices2(1:length(F2),:);
%Fnodes=sortrows(Fnodes,5);
if (length(F2)<length(nodos))
  NFnodes2=devices2(length(F2(:,1))+1:length(nodos),:);
else
  NFnodes2=zeros(1,8);
end

MsgsT_P2=sum(Fnodes2(:,8))

MsgSF72=Fnodes2(1:length(find(Fnodes2(:,6)<8)),:);
%papa=length(find(Fnodes(:,6)>8))
if (length(find(Fnodes2(:,6)>8))>0 && length(find(Fnodes2(:,6)<9)>0))
  MsgSF82=Fnodes2(length(find(Fnodes2(:,6)<8))+1:length(find(Fnodes2(:,6)<9)),:);
  if (length(find(Fnodes2(:,6)>9))>0 && length(find(Fnodes2(:,6)<10)>0))
    MsgSF92=Fnodes2(length(find(Fnodes2(:,6)<9))+1:length(find(Fnodes2(:,6)<10)),:);
    if (length(find(Fnodes2(:,6)>10))>0 && length(find(Fnodes2(:,6)<11)>0))
      MsgSF102=Fnodes2(length(find(Fnodes2(:,6)<10))+1:length(find(Fnodes2(:,6)<11)),:);
      if (length(find(Fnodes2(:,6)>11))>0 && length(find(Fnodes2(:,6)<12)>0))
        MsgSF112=Fnodes2(length(find(Fnodes2(:,6)<11))+1:length(find(Fnodes2(:,6)>12)),:);
        if (length(find(Fnodes2(:,6)>11))>0)
          MsgSF122=Fnodes2(length(find(Fnodes2(:,6)<12))+1:length(Fnodes2(:,6)),:);
        end
      else
        MsgSF112=Fnodes2(length(find(Fnodes2(:,6)<11)+1):length(Fnodes2(:,6)),:);
        MsgSF122=zeros(1,8);
       end
    else
      MsgSF102=Fnodes2(length(find(Fnodes2(:,6)<10)+1):length(Fnodes2(:,6)),:);
      MsgSF112=zeros(1,8);
      MsgSF122=zeros(1,8);
    end
  else
    MsgSF92=Fnodes2(length(find(Fnodes2(:,6)<9)+1):length(Fnodes2(:,6)),:);
    MsgSF102=zeros(1,8);
    MsgSF112=zeros(1,8);
    MsgSF122=zeros(1,8);
    end
else
  MsgSF82=Fnodes2(length(find(Fnodes2(:,6)<8))+1:length(Fnodes2(:,6)),:);
  MsgSF92=zeros(1,8);
  MsgSF102=zeros(1,8);
  MsgSF112=zeros(1,8);
  MsgSF122=zeros(1,8);
end


% Factibilidad
%Acomodo los msgs por SF hasta saturar cada uno. Lo que queda lo sumo a los nodos no factibles.
final2=0
while(final2==0)
  if (sum(MsgSF72(:,8))<=H)
    if (sum(MsgSF82(:,8))<=H/2)
      if (sum(MsgSF92(:,8))<=H/4)
          if (sum(MsgSF102(:,8))<=H/8)
            if (sum(MsgSF112(:,8))<=H/16)
              if (sum(MsgSF122(:,8))<=H/32)
                 factibe2=1;
                 final2=1;
              else
                factible2=0
                i=1;
                while (sum(MsgSF122(1:i,8))<=H/32)
                  i=i+1;
                end
                MsgSF122(i:length(MsgSF122),6)=MsgSF122(i:length(MsgSF122),6)+88;
                NFnodes2=[MsgSF122(i:length(MsgSF122),:);NFnodes2];
                MsgSF122=MsgSF122(1:i-1,:);
                final2=1 
              end
            else
   %           if (sum(MsgSF12(:,8))<=112)
                i=1;
               while (sum(MsgSF112(1:i,8))<=H/16)
                  i=i+1;
                end
                MsgSF112(i:length(MsgSF112),6)=MsgSF112(i:length(MsgSF112),6)+1;
                MsgSF122=[MsgSF112(i:length(MsgSF112),:);MsgSF122];
                MsgSF112=MsgSF112(1:i-1,:);
    %          end
            end  
          else
  %          if (sum(MsgSF11(:,8))<=250)
              i=1;
              while (sum(MsgSF102(1:i,8))<=H/8)
                i=i+1;
              end
              MsgSF102(i:length(MsgSF102),6)=MsgSF102(i:length(MsgSF102),6)+1;
              MsgSF112=[MsgSF102(i:length(MsgSF102),:);MsgSF112];
              MsgSF102=MsgSF102(1:i-1,:);
   %         end
          end  
      else
 %       if (sum(MsgSF10(:,8))<=450)
          i=1;
          while (sum(MsgSF92(1:i,8))<=H/4)
            i=i+1;
          end
          MsgSF92(i:length(MsgSF92),6)=MsgSF92(i:length(MsgSF92),6)+1;
          MsgSF102=[MsgSF92(i:length(MsgSF92),:);MsgSF102];
          MsgSF92=MsgSF92(1:i-1,:);
 %       end
      end
    else
%       if (sum(MsgSF9(:,8))<=900)
          i=1;
          while (sum(MsgSF82(1:i,8))<=H/2)
            i=i+1;
          end
          MsgSF82(i:length(MsgSF82),6)=MsgSF82(i:length(MsgSF82),6)+1;
          MsgSF92=[MsgSF82(i:length(MsgSF82),:);MsgSF92];
          MsgSF82=MsgSF82(1:i-1,:);
 %       end
    end
  else
  %  if (sum(MsgSF8(:,8))<=1800)
      i=1;
      while (sum(MsgSF72(1:i,8))<=H)
        i=i+1;
      end
      MsgSF72(i:length(MsgSF72),6)=MsgSF72(i:length(MsgSF72),6)+1;
      MsgSF82=[MsgSF72(i:length(MsgSF72),:);MsgSF82];
      MsgSF72=MsgSF72(1:i-1,:);
  %  end  
  end
endwhile
%NFnodes=devices(length(F(:,1))+1:eds,:);

SF72=sum(MsgSF72(:,8))
SF82=sum(MsgSF82(:,8))
SF92=sum(MsgSF92(:,8))
SF102=sum(MsgSF102(:,8))
SF112=sum(MsgSF112(:,8))
SF122=sum(MsgSF122(:,8))
NF2=sum(NFnodes2(:,8))
%figure
%scatter(NFnodes2(:,2),NFnodes2(:,3),'.r')
MsgSF7(:,2)=MsgSF7(:,2)%+150;
MsgSF8(:,2)=MsgSF8(:,2)%+150;
MsgSF9(:,2)=MsgSF9(:,2)%+150;
MsgSF10(:,2)=MsgSF10(:,2)%+150;
MsgSF11(:,2)=MsgSF11(:,2)%+150;
MsgSF12(:,2)=MsgSF12(:,2)%+150;

MsgSF72(:,2)=MsgSF72(:,2)%-150;
MsgSF82(:,2)=MsgSF82(:,2)%-150;
MsgSF92(:,2)=MsgSF92(:,2)%-150;
MsgSF102(:,2)=MsgSF102(:,2)%-150;
MsgSF112(:,2)=MsgSF112(:,2)%-150;
MsgSF122(:,2)=MsgSF122(:,2)%-150;

NFnodes2(:,2)=NFnodes2(:,2)%-150;
figure(3)

%scatter(NFnodes2(:,2),NFnodes2(:,3),'.r')

%scatter(NFnodes(:,2),NFnodes(:,3),'.r')
%hold
scatter(MsgSF7(:,2),MsgSF7(:,3),'x b')
hold
scatter(MsgSF8(:,2),MsgSF8(:,3),'o r')
scatter(MsgSF9(:,2),MsgSF9(:,3),'. k')
scatter(MsgSF10(:,2),MsgSF10(:,3),'- y')
scatter(MsgSF11(:,2),MsgSF11(:,3),'x m')
scatter(MsgSF12(:,2),MsgSF12(:,3),'o c')

scatter(MsgSF72(:,2),MsgSF72(:,3),'x b')
scatter(MsgSF82(:,2),MsgSF82(:,3),'o r')
scatter(MsgSF92(:,2),MsgSF92(:,3),'. k')
scatter(MsgSF102(:,2),MsgSF102(:,3),'- y')
scatter(MsgSF112(:,2),MsgSF112(:,3),'x m')
scatter(MsgSF122(:,2),MsgSF122(:,3),'o c')
grid
legend('sf7','sf8','sf9','sf10','sf11','sf12')
figure(4)
scatter(NFnodes2(:,2),NFnodes2(:,3),'.r')
endif