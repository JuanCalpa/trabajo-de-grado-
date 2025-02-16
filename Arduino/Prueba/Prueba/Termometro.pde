class termometro
{
  float posx,posy,diam,alto,minpx,maxpx,valor,valor2,min,max;
  boolean modoHSB;
  int r,g,b,h;
  String magnitud,nombre;
 
  public termometro()
  {
    posx=100;
    posy=100;
    diam=100;
    alto=50;
    modoHSB=false;
    r=255;
    g=0;
    b=0;
    minpx=posy-diam/2;
    maxpx=posy-(alto*0.98);
    valor=50;
    min=0;
    max=100;
    magnitud="C°";
    nombre="Temperatura";
    valor2=2;
  }
  public void setModoHSB(boolean modoHSB)
  {
    this.modoHSB=modoHSB;
  }
  public void setColor(int r,int g,int b)
  {
    this.r=r;
    this.g=g;
    this.b=b;
  }
  public void setDiamAlto(float diam,float alto)
  {
    this.diam=diam;
    this.alto=alto;
  }
  public void posicion(float posx,float posy)
  {
    this.posx=posx;
    this.posy=posy;
  }
  public void setValor(float valor)
  {
    this.valor=valor;
  }
  public void setMinMax(float min,float max)
  {
    this.min=min;
    this.max=max;
  }
  public void nombre(String nombre)
  {
    this.nombre=nombre;
  }
  public void setMagnitud(String magnitud)
  {
    this.magnitud=magnitud;
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
    minpx=posy-diam/2;
    maxpx=posy-(alto*0.98);
    noStroke();
    fill(100);
    ellipse(posx,posy,diam,diam);
    stroke(100);
    strokeWeight(diam/2.5);
    line(posx,posy,posx,posy-alto);
    if(modoHSB)
    {
      noStroke();
      
      colorMode(HSB,360,100,100);
      fill(map(valor2,min,max,227,0),100,100);
      h=int(map(valor2,min,max,227,0));
      ellipse(posx,posy,6*diam/8,6*diam/8);
      stroke(map(valor2,min,max,227,0),100,100);
      strokeWeight((6*diam/8)/3.5);
      line(posx,posy,posx,map(valor2,min,max,minpx,maxpx));
    }
    else
    {
      noStroke();
      colorMode(RGB,255,255,255);
      //fill(r,g,b);
      fill(map(valor2,min,max,0,255),50,100);
      ellipse(posx,posy,6*diam/8,6*diam/8);
      //stroke(r,g,b);
      stroke(map(valor2,min,max,0,255),50,100);
      strokeWeight((6*diam/8)/3.5);
      line(posx,posy,posx,map(valor2,min,max,minpx,maxpx));
      
    }
    fill(#223074);
    textSize(diam*0.3);
    text("Mín: "+str(min)+magnitud,posx+diam/4,posy-diam/2);
    text("Máx: "+str(max)+magnitud,posx+diam/4,posy-(alto*0.98));
    text("Actual: "+(nf(valor2,0,2))+magnitud,posx+diam/4,posy-alto/2-textAscent()/2);
    text(nombre,posx-textWidth(nombre)/2,posy+diam);
  }
  
  public int getColor()
  {
    return h;
  }
}
