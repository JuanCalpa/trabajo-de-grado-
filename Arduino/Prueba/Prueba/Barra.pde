class Barra
{
  float posx, posy, valor, valor2, ancho, alto, min, max;
  String nombre, unidad;
  
  public Barra()
  {
    valor2 = 0;
    valor = 100;
    min = 0;
    max = 100;
    nombre = "NN";
  }
  
  public void posicion(float posx, float posy)
  {
    this.posx=posx;
    this.posy=posy;
  }
  
  public void setAnchoAlto (float ancho, float alto)
  {
    this.ancho=ancho;
    this.alto=alto;
  }
  
  public void setMinMax (float min, float max)
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
  
  public void setValor(float valor){
    this.valor=valor;
  }
  
  public void show()
  {
    if(valor>valor2)
    {
      valor2+=0.2;
    }
    if(valor<valor2)
    {
      valor2-=0.2;
    }
    
    noStroke();
    colorMode(HSB,360,100,100);
    fill(map(valor2,min,max,120,360),91,42);
    rect(posx,posy,ancho,map(valor2,min,max,0,-alto));
    colorMode(RGB,255,255,255);
    
    stroke(1,1,1);
    strokeWeight(3);
    line(posx,posy,posx,posy-alto);
    line(posx,posy,posx+ancho,posy);
    
    strokeWeight(2);
    textSize(0.2*alto*ancho/(alto+ancho));
    textAlign(RIGHT);
    fill(1,1,1);
    for (int i=0; i<11;i++){
      line(posx,posy-map(i*(max-min)/(11-1),min,max,0,alto),posx-ancho*0.1,posy-map(i*(max-min)/(11-1),min,max,0,alto));
      text(str((float)round(i*(max-min)*10/(11-1))/10),posx-ancho*0.2,posy-map(i*(max-min)/(11-1),min,max,0,alto));
    }
    
    textSize(0.3*alto*ancho/(alto+ancho));
    fill(26,105,183);
    textAlign(CENTER);
    text(nombre, posx+ancho/2, posy+0.6*alto*ancho/(alto+ancho));
    textSize(0.2*alto*ancho/(alto+ancho));
    text(unidad, posx+ancho/2, posy+0.82*alto*ancho/(alto+ancho));
    textAlign(LEFT);
  }
}
