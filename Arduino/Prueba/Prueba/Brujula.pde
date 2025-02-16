class Brujula
{
  
  float posx, posy, valor, valor2, diam, min, max, x, y, x1, y1, x2, y2, N;
  int r, g, b;
  String nombre;
  
  public Brujula()
  {
    posx = 10;
    posy = 10;
    valor = 210;
    valor2 = 40;
    diam = 60;
    min = 0;
    max = 360;
    N = 12*10;
    nombre = "Dirección del viento";
  }
  
  public void posicion(float posx,float posy)
  {
    this.posx=posx;
    this.posy=posy;
  }
  
  public void setColor(int r,int g,int b)
  {
    this.r=r;
    this.g=g;
    this.b=b;
  }
  
  public void setDiam(float diam)
  {
    this.diam=diam;
  }
  
  public void setValor(float valor)
  {
    if (valor<min){
      this.valor = min;
    }else if (valor>max){
      this.valor = max;
    }else{
      this.valor=valor;
    }
  }
  
  public void nombre(String nombre)
  {
    this.nombre=nombre;
  }
  
  public void show()
  {
    if(valor-valor2 > 180)
    {
      valor2-=2;
      if (valor2<0)
      {
        valor2=359;
      }
    }else if (valor-valor2 > 0){
      valor2+=2;
    }
    
    if(valor-valor2 < -180)
    {
      valor2+=2;
      if (valor2>360)
      {
        valor2=1;
      }
    }else if (valor-valor2 < 0){
      valor2-=2;
    }
    
    
    fill(r,g,b);
    stroke(r,g,b);
    arc(posx, posy, diam, diam, 0, 2*PI);
    strokeWeight(2);
    stroke(250,250,250);
    //arc(posx, posy, diam*0.95, diam*0.95, 0, 2*PI);
    
    strokeWeight(1);
    for (int i=0;i<N;i=i+1)
    {
      x1 = posx-((0.02-1)*diam*0.9/2)*sin(2*PI*i/(N-1));
      y1 = posy+((0.02-1)*diam*0.9/2)*cos(2*PI*i/(N-1));
      x2 = posx-((-0.02-1)*diam*0.9/2)*sin(2*PI*i/(N-1));
      y2 = posy+((-0.02-1)*diam*0.9/2)*cos(2*PI*i/(N-1));
      line(x1,y1,x2,y2);
    }
    
    strokeWeight(2);
    for (int i=0; i<13; i=i+1)
    {
      x1 = posx-((0.02-1)*diam*0.9/2)*sin(2*PI*i/(13-1));
      y1 = posy+((0.02-1)*diam*0.9/2)*cos(2*PI*i/(13-1));
      x2 = posx-((-0.08-1)*diam*0.9/2)*sin(2*PI*i/(13-1));
      y2 = posy+((-0.08-1)*diam*0.9/2)*cos(2*PI*i/(13-1));
      line(x1,y1,x2,y2);
    }
    
    textAlign(CENTER);
    fill(250,250,250);
    textSize(diam*0.1);
    text("N", posx, posy+(0.25-1)*diam*0.9/2);
    text("E", posx-((0.25-1)*diam*0.9/2), posy);
    text("S", posx, posy-(0.10-1)*diam*0.9/2);
    text("W", posx+((0.25-1)*diam*0.9/2), posy);
    
    x = posx + (0.6*diam/2)*sin(valor2*PI/180);
    y = posy - (0.6*diam/2)*cos(valor2*PI/180);
    x1 = posx+(-0.045*diam)*cos(valor2*PI/180);
    y1 = posy+(-0.045*diam)*sin(valor2*PI/180);
    x2 = posx+(0.045*diam)*cos(valor2*PI/180);
    y2 = posy+(0.045*diam)*sin(valor2*PI/180);
    fill(255,1,1);
    noStroke();
    triangle(x,y,x1,y1,x2,y2);
    x = posx - (0.6*diam/2)*sin(valor2*PI/180);
    y = posy+(0.6*diam/2)*cos(valor2*PI/180);
    fill(175,175,175);
    triangle(x,y,x1,y1,x2,y2);
    
    fill(255,240,64);
    ellipse(posx, posy, diam*0.08, diam*0.08);
    
    fill(1,63,129);
    text(nombre,posx,posy+diam*1.4/2);
    textAlign(LEFT);
  }
}
