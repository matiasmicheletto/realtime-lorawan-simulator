periods=[900,1800,3600];
eds=10;
nodos=[randi(1000,[eds,1]),randi(1000,[eds,1])];
nodos=nodos-500;
nodos=[(1:eds)',nodos,sqrt(nodos(:,1).^2+nodos(:,2).^2),periods(randi(3,eds,1))',zeros(eds,2)];
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
devices=sortrows(devices,4)
F=find(devices(:,6)-100)
Fnodes=devices(1:length(F),:)
Fnodes=sortrows(Fnodes,5);
MsgsT_P=sum(Fnodes(:,8))
Msg900=Fnodes(1:find(Fnodes(:,5)>900)-1,:)
Msg1800=Fnodes(find(Fnodes(:,5)>900):find(Fnodes(:,5)>1800)-1,:)
Msg3600=Fnodes(find(Fnodes(:,5)>1800):length(Fnodes(:,5)),:)

NFnodes=devices(length(F(:,1))+1:eds,:);
