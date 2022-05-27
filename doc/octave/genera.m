function nodesORIG=genera (periods, eds,lado,H)
nodesORIG=[(1:eds)',randi(lado,[eds,1])-lado/2,randi(lado,[eds,1])-lado/2,ones(eds,1),periods(randi(100,eds,1))',zeros(eds,2)];
nodesORIG=[nodesORIG,H./nodesORIG(:,5),zeros(eds,1)];
 
endfunction
