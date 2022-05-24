periods=[900,900,1800,ones(1,997)*3600,];
eds=1000;
nodos=[randi(1000,[eds,1]),randi(1000,[eds,1])];
nodos=nodos-500;
nodos=[(1:eds)',nodos,sqrt(nodos(:,1).^2+nodos(:,2).^2),periods(randi(1000,eds,1))',zeros(eds,2)];
nodos=[nodos,3600./nodos(:,5)];
for i=1:length(nodos(:,1)),
  if (nodos(i,4)<62.5 && nodos(i,5)==900)
    nodos(i,6)=7;
    nodos(i,7)=10;
  elseif (nodos(i,4)<62.5 && nodos(i,5)==1800)
     nodos(i,6)=7;
     nodos(i,7)=11;
  elseif (nodos(i,4)<62.5 && nodos(i,5)==3600)
    nodos(i,6)=7;
    nodos(i,7)=12;
  elseif (nodos(i,4)<125 && nodos(i,5)==900)    
    nodos(i,6)=8;
    nodos(i,7)=10;
  elseif (nodos(i,4)<125 && nodos(i,5)==1800)
    nodos(i,6)=8;
    nodos(i,7)=11;
  elseif (nodos(i,4)<125 && nodos(i,5)==3600)
    nodos(i,6)=8;
    nodos(i,7)=12;
  elseif (nodos(i,4)<250 && nodos(i,5)==900)    
    nodos(i,6)=9;
    nodos(i,7)=10;
  elseif (nodos(i,4)<250 && nodos(i,5)==1800)
    nodos (i,6)=9;
    nodos(i,7)=11;
  elseif (nodos(i,4)<250 && nodos(i,5)==3600)
    nodos(i,6)=9;
    nodos(i,7)=12;
  elseif (nodos(i,4)<500 && nodos(i,5)==900)    
    nodos(i,6)=10;
    nodos(i,7)=10;
  elseif (nodos(i,4)<500 && nodos(i,5)==1800)
    nodos(i,6)=10;
    nodos(i,7)=11;
  elseif (nodos(i,4)<500 && nodos(i,5)==3600)
    nodos(i,6)=10;
    nodos(i,7)=12;
  elseif (nodos(i,4)<1000 && nodos(i,5)==900)    
    nodos(i,6)=100;
    nodos(i,7)=100;
  elseif (nodos(i,4)<1000 && nodos(i,5)==1800)
    nodos(i,6)=11;
    nodos(i,7)=11;
  elseif (nodos(i,4)<1000 && nodos(i,5)==3600)
    nodos(i,6)=11;
    nodos(i,7)=12;
  elseif (nodos(i,4)<2000 && nodos(i,5)==900)    
    nodos(i,6)=100;
    nodos(i,7)=100;
  elseif (nodos(i,4)<2000 && nodos(i,5)==1800)
    nodos(i,6)=100;
    nodos(i,7)=100;
  elseif (nodos(i,4)<2000 && nodos(i,5)==3600)
    nodos(i,6)=12;
    nodos(i,7)=12;
    endif
endfor
devices=sortrows(nodos,6)
%devices=sortrows(devices,4)
F=find(devices(:,6)-100)
Fnodes=devices(1:length(F),:)
Fnodes=sortrows(Fnodes,5);
MsgsT_P=sum(Fnodes(:,8))
MsgSF7=Fnodes(1:find(Fnodes(:,6)>7)-1,:)
MsgSF8=Fnodes(find(Fnodes(:,6)>7):find(Fnodes(:,6)>8)-1,:)
MsgSF9=Fnodes(find(Fnodes(:,6)>8):find(Fnodes(:,6)>9)-1,:)
MsgSF10=Fnodes(find(Fnodes(:,6)>9):find(Fnodes(:,6)>10)-1,:)
MsgSF11=Fnodes(find(Fnodes(:,6)>10):find(Fnodes(:,6)>11)-1,:)
MsgSF12=Fnodes(find(Fnodes(:,6)>11):length(Fnodes(6,:)),:)

% Factibilidad
if (sum(MsgSF7(:,8)<3600))
  if (sum(MsgSF8(:,8)<1800))
    if (sum(MsgSF9(:,8)<900))
      if (sum(MsgSF11(:,8)<450))
         if (sum(MsgSF12(:,8)<225))
           factibe=1
          endif
        endif
      endif
    endif
  endif
NFnodes=devices(length(F(:,1))+1:eds,:);
