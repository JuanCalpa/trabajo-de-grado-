class casilla
{
  
  float posx, posy, valor, valor2, size;
  String nombre, unidad;
  
  public casilla()
  {
    posx = 10;
    posy = 10;
    nombre = "Presión";
    size = 20;
    unidad = "mbar";
    valor = 756;
    valor2 = 700;
  }
  
  public void posicion(float posx,float posy)
  {
    this.posx=posx;
    this.posy=posy;
  }
  
  public void setSize(float size)
  {
    this.size = size;
  }
  
  public void nombre(String nombre)
  {
    this.nombre = nombre;
  }
  
  public void setValor(float valor)
  {
    this.valor = valor;
  }
  
  public void unidad(String unidad)
  {
    this.unidad=unidad;
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
    
    textSize(size);
    text(nombre,posx,posy);
    fill(250,250,250);
    strokeWeight(1);
    stroke(1,1,1);
    rect(posx+textWidth(nombre)+15,posy-size*1.5/1.6,size*4,size*1.3);
    textSize(size*0.7);
    fill(1,1,1);
    text(str(round(valor2*10)/10), posx+textWidth(nombre)/0.7+15+size*4*0.25,posy);
    text(unidad,posx+textWidth(nombre)/0.7+15+100+15,posy);
  }
}
