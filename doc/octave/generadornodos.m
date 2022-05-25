close all; clear all;

%% Parameters
periods=[900,900,1800,ones(1,97)*3600];
eds=6600;
map_size=1000;

H=1;
for i = 1:length(periods),
  H = lcm(H, periods(i));
end
disp(['Hyperperiod ',num2str(H)])

positions=[randi(map_size,[eds,1])-map_size/2, randi(map_size,[eds,1])-map_size/2];
distances = sqrt(positions(:,1).^2 + positions(:,2).^2);
selected_periods = periods(randi(100, eds, 1))';
instances = H./selected_periods;

nodes = [(1:eds)', positions, distances, selected_periods, zeros(eds,2), instances];

% Indexes:
% 1 index
% 2 pos x
% 3 pos y
% 4 dist
% 5 period
% 6 ?
% 7 SF
% 8 instances

%% Plot nodes
scatter(nodes(:,2), nodes(:,3), '.')
title('Positions')



for i = 1:length(nodes),
  if (nodes(i,5)/100 > 32)
    nodes(i,7) = 12;
  elseif (nodes(i,5)/100 > 16)
    nodes(i,7) = 11;
  elseif (nodes(i,5)/100 > 8)
    nodes(i,7) = 10;
  elseif (nodes(i,5)/100 > 4)
    nodes(i,7) = 9;
   elseif (nodes(i,5)/100 > 2)
    nodes(i,7) = 8;
  elseif (nodes(i,5)/100 > 1)
    nodes(i,7) = 7;
  end 
end  

for i = 1:length(nodes(:,1)),
  if (nodes(i,4) < 62.5)
    nodes(i,6) = min(7,nodes(i,7));
  elseif (nodes(i,4) < 125)% && nodes(i,5)==9000)    
    nodes(i,6) = min(8,nodes(i,7));
  elseif (nodes(i,4) < 250)% && nodes(i,5)==9000)    
    nodes(i,6) = min(9,nodes(i,7));
  elseif (nodes(i,4) < 500)% && nodes(i,5)==9000)    
    nodes(i,6) = min(10,nodes(i,7));
  elseif (nodes(i,4) < 1000)% && nodes(i,5)==9000)    
    nodes(i,6) = min(11,nodes(i,7));
  elseif (nodes(i,4) < 2000)% && nodes(i,5)==9000)    
    nodes(i,6) = min(12,nodes(i,7));
  end
end

devices = sortrows(nodes,6);
MsgToT = sum(devices(:,8))
F = find(devices(:,6) < 100);
Fnodes = devices(1:length(F),:);
if (length(F) < eds)
  NFnodes = devices(length(F(:,1))+1:eds,:);
else
  NFnodes = zeros(1,8);
end

MsgsT_P = sum(Fnodes(:,8))
MsgSF7 = Fnodes(1:length(find(Fnodes(:,6)<8)),:);

if (length(find(Fnodes(:,6)>8))>0 && length(find(Fnodes(:,6)<9)>0))
  MsgSF8 = Fnodes(length(find(Fnodes(:,6)<8))+1:length(find(Fnodes(:,6)<9)),:);
  if (length(find(Fnodes(:,6)>9))>0 && length(find(Fnodes(:,6)<10)>0))
    MsgSF9 = Fnodes(length(find(Fnodes(:,6)<9))+1:length(find(Fnodes(:,6)<10)),:);
    if (length(find(Fnodes(:,6)>10))>0 && length(find(Fnodes(:,6)<11)>0))
      MsgSF10 = Fnodes(length(find(Fnodes(:,6)<10))+1:length(find(Fnodes(:,6)<11)),:);
      if (length(find(Fnodes(:,6)>11))>0 && length(find(Fnodes(:,6)<12)>0))
        MsgSF11 = Fnodes(length(find(Fnodes(:,6)<11))+1:length(find(Fnodes(:,6)>12)),:);
        if (length(find(Fnodes(:,6)>11))>0)
          MsgSF12 = Fnodes(length(find(Fnodes(:,6)<12))+1:length(Fnodes(:,6)),:);
        end
      else
        MsgSF11 = Fnodes(length(find(Fnodes(:,6)<11)+1):length(Fnodes(:,6)),:);
        MsgSF12 = zeros(1,8);
       end
    else
      MsgSF10 = Fnodes(length(find(Fnodes(:,6)<10)+1):length(Fnodes(:,6)),:);
      MsgSF11 = zeros(1,8);
      MsgSF12 = zeros(1,8);
    end
  else
    MsgSF9 = Fnodes(length(find(Fnodes(:,6)<9)+1):length(Fnodes(:,6)),:);
    MsgSF10 = zeros(1,8);
    MsgSF11 = zeros(1,8);
    MsgSF12 = zeros(1,8);
    end
else
  MsgSF8 = Fnodes(length(find(Fnodes(:,6)<8))+1:length(Fnodes(:,6)),:);
  MsgSF9 = zeros(1,8);
  MsgSF10 = zeros(1,8);
  MsgSF11 = zeros(1,8);
  MsgSF12 = zeros(1,8);
end


% Factibilidad
%Acomodo los msgs por SF hasta saturar cada uno. Lo que queda lo sumo a los nodes no factibles.
final=0
while(final==0)
  if (sum(MsgSF7(:,8)) <= H)
    if (sum(MsgSF8(:,8)) <= H/2)
      if (sum(MsgSF9(:,8)) <= H/4)
          if (sum(MsgSF10(:,8)) <= H/8)
            if (sum(MsgSF11(:,8)) <= H/16)
              if (sum(MsgSF12(:,8)) <= H/32)
                 factibe=1;
                 final=1;
              else
                factible=0
                i=1;
                while (sum(MsgSF12(1:i,8)) <= H/32)
                  i=i+1;
                end
                MsgSF12(i:length(MsgSF12),6)=MsgSF12(i:length(MsgSF12),6)+88;
                NFnodes=[MsgSF12(i:length(MsgSF12),:);NFnodes];
                MsgSF12=MsgSF12(1:i-1,:);
                final=1 
              end
            else
%             if (sum(MsgSF12(:,8)) <= 112)
                i=1;
                while (sum(MsgSF11(1:i,8)) <= H/16)
                  i=i+1;
                end
                MsgSF11(i:length(MsgSF11),6)=MsgSF11(i:length(MsgSF11),6)+1;
                MsgSF12=[MsgSF11(i:length(MsgSF11),:);MsgSF12];
                MsgSF11=MsgSF11(1:i-1,:);
%              end
            end  
          else
%            if (sum(MsgSF11(:,8)) <= 250)
              i=1;
              while (sum(MsgSF10(1:i,8)) <= H/8)
                i=i+1;
              end
              MsgSF10(i:length(MsgSF10),6)=MsgSF10(i:length(MsgSF10),6)+1;
              MsgSF11=[MsgSF10(i:length(MsgSF10),:);MsgSF11];
              MsgSF10=MsgSF10(1:i-1,:);
%            end
          end  
      else
%     if (sum(MsgSF10(:,8)) <= 450)
          i=1;
          while (sum(MsgSF9(1:i,8)) <= H/4)
            i=i+1;
          end
          MsgSF9(i:length(MsgSF9),6)=MsgSF9(i:length(MsgSF9),6)+1;
          MsgSF10=[MsgSF9(i:length(MsgSF9),:);MsgSF10];
          MsgSF9=MsgSF9(1:i-1,:);
%     end
      end
    else
%       if (sum(MsgSF9(:,8)) <= 900)
          i=1;
          while (sum(MsgSF8(1:i,8)) <= H/2)
            i=i+1;
          end
          MsgSF8(i:length(MsgSF8),6)=MsgSF8(i:length(MsgSF8),6)+1;
          MsgSF9=[MsgSF8(i:length(MsgSF8),:);MsgSF9];
          MsgSF8=MsgSF8(1:i-1,:);
%       end
    end
  else
%     if (sum(MsgSF8(:,8)) <= 1800)
      i=1;
      while (sum(MsgSF7(1:i,8)) <= H)
        i=i+1;
      end
      MsgSF7(i:length(MsgSF7),6)=MsgSF7(i:length(MsgSF7),6)+1;
      MsgSF8=[MsgSF7(i:length(MsgSF7),:);MsgSF8];
      MsgSF7=MsgSF7(1:i-1,:);
%     end  
  end
end
%NFnodes=devices(length(F(:,1))+1:eds,:);
SF7=sum(MsgSF7(:,8))
SF8=sum(MsgSF8(:,8))
SF9=sum(MsgSF9(:,8))
SF10=sum(MsgSF10(:,8))
SF11=sum(MsgSF11(:,8))
SF12=sum(MsgSF12(:,8))
NF=sum(NFnodes(:,8))

scatter(NFnodes(:,2),NFnodes(:,3),'.r')

figure
scatter(NFnodes(:,2),NFnodes(:,3),'.r')
hold
scatter(MsgSF7(:,2),MsgSF7(:,3),'xb')
scatter(MsgSF8(:,2),MsgSF8(:,3),'or')
scatter(MsgSF9(:,2),MsgSF9(:,3),'.k')
scatter(MsgSF10(:,2),MsgSF10(:,3),'-y')
scatter(MsgSF11(:,2),MsgSF11(:,3),'xm')
scatter(MsgSF12(:,2),MsgSF12(:,3),'oc')
grid
legend('nf','SF7','SF8','SF9','SF10','SF11','SF12')
