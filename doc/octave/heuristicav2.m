%Version 2 para ver si se puede generar una pobleción de gws para
%seleccionar entre ellos los que minimicen.
%por ejemplo dejando gws en diferentes puntos y encendiendolos para verificar
%el nivel de asociatividad con el. 
%Una posibilidad trivial seria empezar asignando en cada gw y continuar hasta 
%terminar. Se cuentan los gw usados. Luego empezamos con el segundo y terminamos
%en el primero, luego con el tercero, etc. Primera pasada. En la segunda pasada, 
%elegimos a los gws que hayan tenido mayor asociatividad en las pasadas anteriores
% y luego ... 
%Otra posibilidad es armar una PSO o GA con el cromosoma que tenga los GW y ponerles
%valor binario 1,0 según estén prendidos o apagados y hacerlo evolucionar.

function nodosFIN = heuristicav2 (iter,eds,lado)
  periods=[900,900,1800,ones(1,97)*3600];
  H=1;
  for h=1:length(periods),
    H=lcm(H,periods(h));
  endfor
  nodosORIG = genera(periods,eds,lado,H);
  canal=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];
  sol=[0];
   
  %genera vector de gws
  GW=gateways(lado)
for it=1:length(GW(:,1)),
  nodosH=nodosORIG;
  nodosP=[];
  ch=1;
  falta=0;
  paso=0;
  old=0;
  new=0;
  posB=[0,0];
  h=it;
  
  while (ch<17 && falta==0)
    %muevo la posicion del gw buscando que asigne la mayor cantidad
    %pos=[(-1)^(randi(2))*randi(lado/2),(-1)^(randi(2))*randi(lado/2)];
    while (h<length(GW(:,1))+1)
      pos=[GW(h,1),GW(h,2)]
      if (size(nodosH)>0)
         nodosP = asigna (pos,nodosH,canal(ch),H);
      endif
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
        figure
        scatter(nodosH(:,2),nodosH(:,3))
        falta=1;
        h=length(GW(:,1))+1;
      endif 
      h=h+1     
     endwhile
      if (size(nodosFIN)>=size(nodosORIG))
        falta=1;
        h=length(GW(:,1))+1;
      endif 
     sol=[sol,ch-1]
   endwhile
  endfor
endfunction
