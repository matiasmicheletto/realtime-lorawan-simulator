function GW = gateways(lado)
    GW=[-lado/2+125,lado/2-125];
  i=2
  j=1
  while (lado/2-j*125>-lado/2)
      while (-lado/2+i*125<lado/2)
        GW=[GW;[-lado/2+i*125,lado/2-j*125]];
        i=i+1;
      endwhile
      j=j+1;
      i=1;
  endwhile

endfunction
