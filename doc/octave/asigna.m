function A = asigna (pos,nodos,canal,H)

nodos(:,2)=nodos(:,2)-pos(1)/2;
nodos(:,3)=nodos(:,3)-pos(2)/2;
nodos(:,4)=sqrt(nodos(:,2).^2+nodos(:,3).^2);
eds=length(nodos);
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
  NFnodes=zeros(1,9);
end

MsgsT_P=sum(Fnodes(:,9))

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
        MsgSF12=zeros(1,9);
       end
    else
      MsgSF10=Fnodes(length(find(Fnodes(:,6)<10)+1):length(Fnodes(:,6)),:);
      MsgSF11=zeros(1,9);
      MsgSF12=zeros(1,9);
    end
  else
    MsgSF9=Fnodes(length(find(Fnodes(:,6)<9)+1):length(Fnodes(:,6)),:);
    MsgSF10=zeros(1,9);
    MsgSF11=zeros(1,9);
    MsgSF12=zeros(1,9);
    end
else
  MsgSF8=Fnodes(length(find(Fnodes(:,6)<8))+1:length(Fnodes(:,6)),:);
  MsgSF9=zeros(1,9);
  MsgSF10=zeros(1,9);
  MsgSF11=zeros(1,9);
  MsgSF12=zeros(1,9);
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
Fnodes=[MsgSF7;MsgSF8;MsgSF9;MsgSF10;MsgSF11;MsgSF12];
Fnodes(:,2)=Fnodes(:,2)+pos(1)/2;
Fnodes(:,3)=Fnodes(:,3)+pos(2)/2;
Fnodes(:,9)=canal;

NFnodes(:,2)=NFnodes(:,2)+pos(1)/2;
NFnodes(:,3)=NFnodes(:,3)+pos(2)/2;
A=[Fnodes;NFnodes];
SF7=sum(MsgSF7(:,8));
SF8=sum(MsgSF8(:,8));
SF9=sum(MsgSF9(:,8));
SF10=sum(MsgSF10(:,8));
SF11=sum(MsgSF11(:,8));
SF12=sum(MsgSF12(:,8));
NF=sum(NFnodes(:,8))
scatter(Fnodes(:,2),Fnodes(:,3))  

endfunction
