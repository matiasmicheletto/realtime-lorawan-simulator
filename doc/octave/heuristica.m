function nodosFIN = heuristica (iter,eds,lado)
  periods=[900,900,1800,ones(1,97)*3600];
  H=1;
  for h=1:length(periods),
    H=lcm(H,periods(h));
  endfor
  nodosORIG = genera(periods,eds,lado,H);
  canal=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];
  nodosH=nodosORIG; 
  ch=1;
  falta=1;
  paso=0;
  old=0;
  new=0;
  posB=[0,0];
  GW=[posB];
  while (ch<17 && falta==1)
    %muevo la posicion del gw buscando que asigne la mayor cantidad
    pos=[(-1)^(randi(2))*randi(lado/2),(-1)^(randi(2))*randi(lado/2)];
    for it=1:iter,
      if (size(nodosH)>0)
         nodosP = asigna (pos,nodosH,canal(ch),H);
      endif
      F=find(nodosP(:,6)<100);
      new=length(F);
      if (new>old)
        posB=pos;
        nodosN=nodosP;
        old=new;
      endif
      pos=[(-1)^(randi(2))*randi(lado/2),(-1)^(randi(2))*randi(lado/2)];
    endfor
    old=0;
    new=0;
    GW=[GW;posB]
    nodosP=nodosN;
      F=find(nodosP(:,6)<100);
      if (length(F)>0)
        if (paso==0)
          nodosFIN=[nodosP(1:length(F),:)];
          paso=1;
        else
          nodosFIN=[nodosFIN;nodosP(1:length(F),:)];
        endif  
        nodosH=nodosP(length(F)+1:length(nodosP),:);
        %nodosH=nodosORIG;
        ch=ch+1;
        figure
        scatter(nodosH(:,2),nodosH(:,3))       
      else
        falta=0
      endif
      if (size(nodosFIN)>=size(nodosORIG))
        break;
        figure
        scatter(nodosH(:,2),nodosH(:,3))
      endif    
  endwhile
  GW
  scatter(GW(:,1),GW(:,2))
  ch
endfunction
