class pHmetro
{
  float posx, posy, base, alto, valor, valor2;
  
  public pHmetro()
  {
    posx=10;
    posy=10;
    base=10;
    alto=5;
    valor=14;
    valor2=1;
  }
  
  public void posicion(float posx, float posy)
  {
    this.posx=posx;
    this.posy=posy;
  }
  
  public void setBaseAlto(float base, float alto)
  {
    this.base=base;
    this.alto=alto;
  }
  
  public void setValor(float valor)
  {
    this.valor=valor;
  }
  
  public void show()
  {
    if(valor>valor2)
    {
      valor2+=0.1;
    }
    if(valor<valor2)
    {
      valor2-=0.1;
    }
    
    noStroke();
    fill(175,2,8);
    rect(posx, posy, base/14, alto,0);
    fill(175,57,2);
    rect(posx+base/14, posy, base/14, alto,0);
    fill(175,89,2);
    rect(posx+2*base/14, posy, base/14, alto,0);
    fill(227,125,23);
    rect(posx+3*base/14, posy, base/14, alto,0);
    fill(227,183,23);
    rect(posx+4*base/14, posy, base/14, alto,0);
    fill(249,255,98);
    rect(posx+5*base/14, posy, base/14, alto,0);
    fill(222,206,161);
    rect(posx+6*base/14, posy, base/14, alto,0);
    fill(152,142,115);
    rect(posx+7*base/14, posy, base/14, alto,0);
    fill(142,152,72);
    rect(posx+8*base/14, posy, base/14, alto,0);
    fill(68,77,68);
    rect(posx+9*base/14, posy, base/14, alto,0);
    fill(85,33,170);
    rect(posx+10*base/14, posy, base/14, alto,0);
    fill(80,67,79);
    rect(posx+11*base/14, posy, base/14, alto,0);
    fill(67,45,66);
    rect(posx+12*base/14, posy, base/14, alto,0);
    fill(72,48,60);
    rect(posx+13*base/14, posy, base/14, alto,0);
    
    fill(1,1,1);
    rect(posx, posy+alto*1.8, base, alto*0.1, 0);
    triangle(posx-10+base*(valor2-0.5)/14,posy+alto*1.7,posx+base*(valor2-0.5)/14,posy+alto*1.4,posx+10+base*(valor2-0.5)/14,posy+alto*1.7);
    text(str((float)round(valor2*10)/10), posx-10+base*(valor2-0.5)/14, posy+alto*2.5);
    
    textSize(alto*0.4);
    text("pH",posx-base*0.08,posy+alto*0.7);
  }
}
