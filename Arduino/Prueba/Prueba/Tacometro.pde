class Tacometro
{
  
  float posx, posy, diam, valor, valor2, x, y, x1, y1, x2, y2, M, L, min, max;
  int r, g, b, N;
  String nombre, unidad;
  
  public Tacometro()
  {
    posx = 100;
    posy = 100;
    r = 247;
    g = 15;
    b = 15;
    diam = 150;
    min = 0;
    max = 50;
    N = 5;
    nombre = "Humedad Aire";
    unidad = "%";
    valor = max;
    valor2 = min;
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
  
  public void setMinMax(float min,float max)
  {
    this.min=min;
    this.max=max;
  }
  
  public void nombre(String nombre)
  {
    this.nombre=nombre;
  }
  
  public void unidad(String unidad)
  {
    this.unidad=unidad;
  }
  
  public void setMuestra(int N)
  {
    this.N = N;
  }
  
  public void show()
  {
    if(valor>valor2)
    {
      valor2+=0.3;
    }
    if(valor<valor2)
    {
      valor2-=0.3;
    }
    
    noFill();
    strokeWeight(7);
    stroke(r,g,b);
    arc(posx, posy, diam*0.5, diam*0.5, 5*PI/6, 13*PI/6);
    arc(posx, posy, diam, diam, 5*PI/6, 13*PI/6);
    strokeWeight(3);
    stroke(227,55,55);
    x = posx + (-sqrt(3)*diam/8)*cos(4*PI*(valor2-min)/(3*(max-min)))-(diam/8)*sin(4*PI*(valor2-min)/(3*(max-min)));
    y = posy + (-sqrt(3)*diam/8)*sin(4*PI*(valor2-min)/(3*(max-min)))+(diam/8)*cos(4*PI*(valor2-min)/(3*(max-min)));
    L = 0.05*diam;
    M = sqrt(pow(diam/4,2)+pow(L,2)-sqrt(3)*L*diam/4);
    x1 = posx + 0.5*(-sqrt(3)*diam/4+L)*cos(4*PI*(valor2-min)/(3*(max-min)))-0.5*(diam/4-sqrt(3)*L)*sin(4*PI*(valor2-min)/(3*(max-min)));
    y1 = posy + 0.5*(-sqrt(3)*diam/4+L)*sin(4*PI*(valor2-min)/(3*(max-min)))+0.5*(diam/4-sqrt(3)*L)*cos(4*PI*(valor2-min)/(3*(max-min)));
    x2 = posx + 0.5*(-sqrt(3)*diam/4+2*L)*cos(4*PI*(valor2-min)/(3*(max-min)))-0.5*(diam/4)*sin(4*PI*(valor2-min)/(3*(max-min)));
    y2 = posy + 0.5*(-sqrt(3)*diam/4+2*L)*sin(4*PI*(valor2-min)/(3*(max-min)))+0.5*(diam/4)*cos(4*PI*(valor2-min)/(3*(max-min)));
    line(posx,posy,x,y);
    triangle(x,y,x1,y1,x2,y2);
    
    //strokeWeight(1);
    fill(1,1,1);
    textSize(diam*0.06);
    textAlign(CENTER);
    
    for (int i=0; i<N; i=i+1){
      x = posx + (-sqrt(3)*diam/5)*cos(4*PI*i/(3*(N-1)))-(diam/5)*sin(4*PI*i/(3*(N-1)));
      y = posy + (-sqrt(3)*diam/5)*sin(4*PI*i/(3*(N-1)))+(diam/5)*cos(4*PI*i/(3*(N-1)));
      text(str((float)round((max-min)*i*10/(N-1)+min)/10), x, y);
    }
    
    text(str((float)round(valor2*10)/10), posx, posy+0.18*diam);
    text(unidad, posx, posy+0.3*diam);
    //fill(227,55,55);
    noFill();
    rect(posx-0.15*diam, posy+0.1*diam, 0.30*diam, 0.1*diam);
    rect(posx-0.15*diam, posy+0.23*diam, 0.30*diam, 0.1*diam);
    textSize(diam*0.08);
    fill(r,g,b);
    text(nombre, posx, posy+0.45*diam);
    textAlign(LEFT);
  }
}
