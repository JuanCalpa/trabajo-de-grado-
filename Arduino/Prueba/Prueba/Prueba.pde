import processing.serial.*;
import grafica.*;
//import controlP5.*;
import g4p_controls.*;
import java.awt.Font;
 import java.awt.*;


//DEFINICIÓN DE VARIABLES

public GWindow windowAyuda, windowPuertos;
public PFont font;
public GButton btnSaveCOM, btnStopCOM;
public GLabel LabelAyuda, LabelPuerto, LabelPuerto1, LabelPuerto2, LabelPuerto3, LabelNum;
public Serial miPuerto, miPuerto1;
public GDropList COMList;
public GPlot plot1, plot2, plot3, plot4;

termometro TAire = new termometro();
Tacometro Hum_Aire = new Tacometro();
casilla Presion = new casilla();
Tacometro Irradiancia = new Tacometro();
Tacometro Vel_viento = new Tacometro();
Brujula Dir_viento = new Brujula();

termometro TSuelo = new termometro();
Tacometro Hum_Suelo = new Tacometro();
Tacometro Cond_Elec = new Tacometro();
pHmetro pH = new pHmetro();
Barra Nitro = new Barra();
Barra Fosfo = new Barra();
Barra Pota = new Barra();

int ax = 1850, ay = 970, ancho = 90, alto = 30, cx = 860,cy = 62;
int valora;
float Temp_air, Hum_air, Pres_air, Irra, CO, PM1, PM25, PM10, Vel_wind, Dir_wind, Temp_soil, Hum_soil, pH_soil, Sigma, Nitrogeno, Fosforo, Potasio;
boolean pressboton = false, Connected = false, SD_Connected = false;
boolean ON_Ayuda = false, ON_Puertos = false, ON_Datos = false, ON_Save = false;
String COM = "COM9";
String[] lines;
public int j = 0;
public char chByte;
public float Frec_Muestreo = 1, RETRASO = 0.20*Frec_Muestreo;


void setup(){
  size(1850,970);
  background(255);
  lines = loadStrings("COMDefault.txt");
  COM = lines[0].substring(0,lines[0].length());
  println(COM);
  //println(Serial.list());
  windowAyuda =  GWindow.getWindow(this, "Ayuda", 750, 350, 480, 320, JAVA2D);
  windowAyuda.setVisible(false);
  windowPuertos =  GWindow.getWindow(this, "Configuración de Puerto Bluetooth", 750, 350, 480, 320, JAVA2D);
  windowPuertos.setVisible(false);
  LabelPuerto = new GLabel(windowPuertos,0,0,450,100,"BIENVENIDO \n"+"Revise en la sección de 'Bluetooth y otros dispositivos' de su computadora con el fin de identificar el puerto COM con el que se va a desarrollar la comunicación");
  LabelPuerto.setFont(new Font("Simplex", Font.PLAIN, 16));
  LabelPuerto1 = new GLabel(windowPuertos,130,110,300,30,"Seleccionar el puerto COM:");
  LabelPuerto1.setFont(new Font("Monospaced.bold-24", Font.PLAIN, 16));
  LabelPuerto2 = new GLabel(windowPuertos,20,100,250,150,"Puerto por Default:");
  LabelPuerto2.setFont(new Font("YuGothic-Light", Font.PLAIN, 16));
  LabelPuerto3 = new GLabel(windowPuertos,200,100,250,150,COM);
  LabelPuerto3.setFont(new Font("Monospaced.bold-24", Font.PLAIN, 16));
  COMList = new GDropList(windowPuertos, 150, 220, 200, 80);
  COMList.setItems(Serial.list(), 1);
  COMList.addEventHandler(this, "COMList_click");
  btnSaveCOM = new GButton(windowPuertos, 40, 250, 100, 40, "Guardar y conectar");
  btnSaveCOM.addEventHandler(this, "SaveCOM");
  btnStopCOM = new GButton(windowPuertos, 200, 250, 100, 40, "Desconectar");
  btnStopCOM.addEventHandler(this, "StopCOM");
  //miPuerto1 = new Serial(this, "COM4", 9600);  
  
  
  TAire.posicion(ax*7/100,ay*25/100);
  TAire.setDiamAlto(40,85);
  TAire.setMinMax(-10,60);
  TAire.nombre("Temperatura Aire");
  
  Hum_Aire.posicion(ax*20/100,ay*23/100);
  Hum_Aire.setColor(15,75,247);
  Hum_Aire.setDiam(160);
  Hum_Aire.setMuestra(9);
  Hum_Aire.setMinMax(0,100);
  Hum_Aire.nombre("Humedad Aire");
  Hum_Aire.unidad(" % ");
  
  Irradiancia.posicion(ax*32/100,ay*23/100);
  Irradiancia.setColor(15,75,247);
  Irradiancia.setDiam(160);
  Irradiancia.setMuestra(7);
  Irradiancia.setMinMax(0,800);
  Irradiancia.nombre("Irradiancia");
  Irradiancia.unidad(" W/m2 ");
  
  Presion.posicion(ax*2/100,ay*35/100);
  Presion.nombre("Presión");
  Presion.setSize(25);
  
  Vel_viento.posicion(ax*45/100,ay*23/100);
  Vel_viento.setColor(15,75,247);
  Vel_viento.setDiam(160);
  Vel_viento.setMuestra(9);
  Vel_viento.setMinMax(0,50);
  Vel_viento.nombre("Velocidad viento");
  Vel_viento.unidad(" m/s ");
  
  Dir_viento.posicion(ax*54/100,ay*58/100);
  Dir_viento.setColor(10,10,10);
  Dir_viento.setDiam(170);
  Dir_viento.nombre("Dirección viento");
  
  plot1=start(plot1,ax*1/100,ay*38/100,0,1500,320,190,"Concentración de Monóxido de Carbono CO","Tiempo (s)","ppm");
  plot2=start(plot2,ax*25/100,ay*38/100,0,500,320,190,"Concentración de PM1","Tiempo (s)","ug/cm");
  plot3=start(plot3,ax*1/100,ay*69/100,0,500,320,190,"Concentración de PM2.5","Tiempo (s)","ug/cm");
  plot4=start(plot4,ax*25/100,ay*69/100,0,500,320,190,"Concentración de PM10","Tiempo (s)","ug/cm");
  //plot1.addDrawHandler(this,"win_draw");
  
  TSuelo.posicion(ax*67/100,ay*25/100);
  TSuelo.setDiamAlto(40,85);
  TSuelo.setMinMax(-10,60);
  TSuelo.nombre("Temperatura Suelo");
  
  Hum_Suelo.posicion(ax*80/100,ay*23/100);
  Hum_Suelo.setColor(15,75,247);
  Hum_Suelo.setDiam(160);
  Hum_Suelo.setMuestra(9);
  Hum_Suelo.setMinMax(0,100);
  Hum_Suelo.nombre("Humedad Suelo");
  Hum_Suelo.unidad(" % ");
  
  Cond_Elec.posicion(ax*92/100,ay*23/100);
  Cond_Elec.setColor(15,75,247);
  Cond_Elec.setDiam(160);
  Cond_Elec.setMuestra(9);
  Cond_Elec.setMinMax(0,2000);
  Cond_Elec.nombre("Conductividad eléctrica");
  Cond_Elec.unidad(" mS/cm ");
  
  pH.posicion(ax*65/100,ay*38/100);
  pH.setBaseAlto(560,40);
  
  Nitro.posicion(ax*72/100,ay*85/100);
  Nitro.setAnchoAlto(80,300);
  Nitro.setMinMax(0,100);
  Nitro.nombre("Nitrogeno");
  Nitro.unidad("mg/kg");
  
  Fosfo.posicion(ax*80/100,ay*85/100);
  Fosfo.setAnchoAlto(80,300);
  Fosfo.setMinMax(0,100);
  Fosfo.nombre("Fosforo");
  Fosfo.unidad("mg/kg");
  
  Pota.posicion(ax*88/100,ay*85/100);
  Pota.setAnchoAlto(80,300);
  Pota.setMinMax(0,100);
  Pota.nombre("Potasio");
  Pota.unidad("mg/kg");
  
}

void draw(){
  background(198,191,182);
  Nomenclatura();
  botonPuertos();
  botonInit();
  botonStop();
  botonSave();
  botonStopSave();
  
  TAire.show();
  Hum_Aire.show();
  Presion.show();
  Irradiancia.show();
  Vel_viento.show();
  Dir_viento.show();
  
  win_draw();
  
  TSuelo.show();
  Hum_Suelo.show();
  Cond_Elec.show();
  pH.show();
  Nitro.show();
  Fosfo.show();
  Pota.show();
  
  
  if (ON_Datos == true){
    TAire.setValor(Temp_air);
    Hum_Aire.setValor(Hum_air);
    Presion.setValor(Pres_air);
    Irradiancia.setValor(Irra);
    Vel_viento.setValor(Vel_wind);
    Dir_viento.setValor(Dir_wind);
    TSuelo.setValor(Temp_soil);
    Hum_Suelo.setValor(Hum_soil);
    Cond_Elec.setValor(Sigma);
    pH.setValor(pH_soil);
    Nitro.setValor(Nitrogeno);
    Fosfo.setValor(Fosforo);
    Pota.setValor(Potasio);
  }
}

///////////////////////////* EVENTOS*////////////////////////////

public void COMList_click(GDropList list, GEvent event){
  COM = COMList.getSelectedText();
  LabelPuerto3.setText(COMList.getSelectedText());
  println(COMList.getSelectedText());
  windowPuertos.background(204,209,209);
}

public void SaveCOM(GButton source, GEvent event) {
  lines[0] = COM;
  saveStrings("COMDefault.txt", lines);
  println(COM);
  //windowPuertos.background(255,152,0);
  try{
    if (Connected == false){
      miPuerto = new Serial(this, "COM9", 9600);
      chByte = miPuerto.readChar();
      println(chByte);
      miPuerto.bufferUntil('\n');
      serialEvent(miPuerto);
      Connected = true;
    }else{
      println("Conection busy by: ",COM);
    }
  }catch(Exception e){
    println("caught error connecting:");
    e.printStackTrace();
  }
  
  try{
    
      miPuerto1 = new Serial(this, "COM4", 9600);
      chByte = miPuerto1.readChar();
      println(chByte);
      miPuerto1.bufferUntil('\n');
    
  }catch(Exception e){
    println("caught error connecting:");
    e.printStackTrace();
  }
}

public void StopCOM(GButton source, GEvent event){
  if (Connected == true){
    Connected = false;
    miPuerto.stop();
  }
}

void win_draw() {
  if (plot1 != null) {
    //plot1.defaultDraw();
    plot1.beginDraw();
    plot1.drawBackground();
    plot1.drawBox();
    plot1.drawXAxis();
    plot1.drawYAxis();
    plot1.drawTitle();
    plot1.drawGridLines(GPlot.BOTH);
    plot1.drawLines();
    plot1.endDraw();
  }
  
  if (plot2 != null) {
    //plot2.defaultDraw();
    plot2.beginDraw();
    plot2.drawBackground();
    plot2.drawBox();
    plot2.drawXAxis();
    plot2.drawYAxis();
    plot2.drawTitle();
    plot2.drawGridLines(GPlot.BOTH);
    plot2.drawLines();
    plot2.endDraw();
  }
  
  if (plot3 != null) {
    //plot3.defaultDraw();
    plot3.beginDraw();
    plot3.drawBackground();
    plot3.drawBox();
    plot3.drawXAxis();
    plot3.drawYAxis();
    plot3.drawTitle();
    plot3.drawGridLines(GPlot.BOTH);
    plot3.drawLines();
    plot3.endDraw();
  }
  
  if (plot4 != null) {
    //plot4.defaultDraw();
    plot4.beginDraw();
    plot4.drawBackground();
    plot4.drawBox();
    plot4.drawXAxis();
    plot4.drawYAxis();
    plot4.drawTitle();
    plot4.drawGridLines(GPlot.BOTH);
    plot4.drawLines();
    plot4.endDraw();
  }
  
  if (ON_Datos == true){
    pintarpuntos();
   }
}


void serialEvent (Serial miPuerto) {
  try{
    //println("aqui estoy");
    if (ON_Datos == true){
       String inString =miPuerto.readStringUntil('\n');
       if (inString!= null) {
         inString = trim(inString);
         println(inString);
       }
       if(inString == null){
       }
       
       if (inString.charAt(0)=='A') {
         if(inString.charAt(1)=='-'){
           valora=int(inString.substring(2, inString.length()));
           Temp_air = ((float)valora/10)*(-1);
         }else{
           valora=int(inString.substring(1, inString.length()));
           Temp_air = (float)valora/10;
         }
         
       //   //impresiona[j]=Temperatura_frio;
       //   //println(impresiona[j]);
       } else if(inString.charAt(0)=='B'){
         valora=int(inString.substring(1, inString.length()));
         Hum_air = (float)valora/10;
  
       //  //impresionb[j]=Temperatura_caliente;
       //  //println(Temperatura_caliente);
       //  //j++;
       //  //t2 = millis();
       //  //tiempo = t2-t1;
       //  //if(tiempo > 40000){
       //  //  band1 = false;
       //  //  band3 = true;
       //  //  miPuerto.write('P');
       //  //}
       }else if(inString.charAt(0)=='C'){
         valora=int(inString.substring(1, inString.length()));
         Pres_air = (float)valora;
       //  ON_Datos = false;
       //  //mostrar = true;
       //  //Proceso = 0;
       //  //valorstop = Temperatura_frio;
       }else if(inString.charAt(0)=='D'){
         valora=int(inString.substring(1, inString.length()));
         CO = (float)valora/10;
         j++;
       }else if(inString.charAt(0)=='E'){
         valora=int(inString.substring(1, inString.length()));
         PM1 = (float)valora;
       }else if(inString.charAt(0)=='F'){
         valora=int(inString.substring(1, inString.length()));
         PM25 = (float)valora;
       }else if(inString.charAt(0)=='G'){
         valora=int(inString.substring(1, inString.length()));
         PM10 = (float)valora;
       }else if(inString.charAt(0)=='H'){
         valora=int(inString.substring(1, inString.length()));
         Irra = (float)valora;
       }else if(inString.charAt(0)=='I'){
         valora=int(inString.substring(1, inString.length()));
         Vel_wind = (float)valora;
       }else if(inString.charAt(0)=='J'){
         valora=int(inString.substring(1, inString.length()));
         Dir_wind = (float)valora;
       }else if(inString.charAt(0)=='L'){
         if(inString.charAt(1)=='-'){
           valora=int(inString.substring(2, inString.length()));
           Temp_soil = ((float)valora/10)*(-1);
         }else{
           valora=int(inString.substring(1, inString.length()));
           Temp_soil = (float)valora/10;
         }
       }else if(inString.charAt(0)=='M'){
         valora=int(inString.substring(1, inString.length()));
         Hum_soil = (float)valora;
       }else if(inString.charAt(0)=='O'){
         valora=int(inString.substring(1, inString.length()));
         pH_soil = (float)valora/100;
       }else if(inString.charAt(0)=='Q'){
         valora=int(inString.substring(1, inString.length()));
         Sigma = (float)valora;
       }else if(inString.charAt(0)=='N'){
         valora=int(inString.substring(1, inString.length()));
         Nitrogeno = (float)valora;
       }else if(inString.charAt(0)=='P'){
         valora=int(inString.substring(1, inString.length()));
         Fosforo = (float)valora;
       }else if(inString.charAt(0)=='K'){
         valora=int(inString.substring(1, inString.length()));
         Potasio = (float)valora;
       }else if(inString.charAt(0)=='X'){
         SD_Connected = false;
       }else if(inString.charAt(0)=='Y'){
         SD_Connected = true;
       }
       println(valora);
    }
  }catch(Exception e){
      println("caught error parsing data:");
      e.printStackTrace();
  }
}

////////////////////////////////* PLOTS *//////////////////////////////////

void Nomenclatura(){
  textSize(30);
  fill(15, 35, 134);
  font = loadFont("TimesNewRomanPSMT-48.vlw");
  textFont(font);
  text("Aire", ax*2/100, ay*12/100);
  text("Suelo", ax*60/100, ay*12/100);
  fill(250,250,250);
  strokeWeight(1);
  stroke(1,1,1);
  rect(ax*0.6/100,ay*5/100,100,20);
  fill(1,1,1);
  textSize(15);
  if (Connected == true){
    text("Conectado",ax*0.9/100,ay*6.5/100);
  }else{
    text("No Conectado",ax*0.9/100,ay*6.5/100);
  }
  
  fill(250,250,250);
  strokeWeight(1);
  stroke(1,1,1);
  rect(ax*52/100,ay*10/100,125,20);
  fill(1,1,1);
  textSize(15);
  if (SD_Connected == true){
    text("SD Disponible",ax*53/100,ay*11.5/100);
  }else{
    text("SD No Disponible",ax*52.5/100,ay*11.5/100);
  }
}


GPlot start(GPlot plot, int posx,int posy , int limx, int limy, int dimx, int dimy, String txprin, String txx, String txy){
    plot = new GPlot(this);
    plot.setPos(posx, posy);
    plot.setYLim(limx,limy);
    plot.setDim(dimx, dimy);
    plot.getTitle().setText(txprin);
    plot.getXAxis().getAxisLabel().setText(txx);
    plot.getYAxis().getAxisLabel().setText(txy);
    plot.getYAxis().setRotateTickLabels(false);
    plot.activateZooming(1.5);
    return plot;
}

public void  pintarpuntos(){ 
   GPoint lastPoint1 = plot1.getPointsRef().getLastPoint();
   GPoint lastPoint2 = plot2.getPointsRef().getLastPoint();
   GPoint lastPoint3 = plot3.getPointsRef().getLastPoint();
   GPoint lastPoint4 = plot4.getPointsRef().getLastPoint();
   
    //if(ON_Datos==true){
          if (lastPoint1 == null) {
            stroke(100);
            plot1.addPoint(0,0);
            stroke(100);
          } else if (lastPoint1.isValid()) {
            plot1.addPoint((float)j,CO);
            // movimiento del plot
            if (j>50) {
                plot1.moveHorizontalAxesLim(RETRASO);  
            }
          }
          
          if (lastPoint2 == null) {
            stroke(100);
            plot2.addPoint(0,0);
            stroke(100);
          } else if (lastPoint2.isValid()) {
            plot2.addPoint((float)j,PM1);
            // movimiento del plot
            if (j>50) {
                plot2.moveHorizontalAxesLim(RETRASO);  
            }
          }
          
          if (lastPoint3 == null) {
            stroke(100);
            plot3.addPoint(0,0);
            stroke(100);
          } else if (lastPoint3.isValid()) {
            plot3.addPoint((float)j,PM25);
            // movimiento del plot
            if (j>50) {
                plot3.moveHorizontalAxesLim(RETRASO);  
            }
          } 
          
          if (lastPoint4 == null) {
            stroke(100);
            plot4.addPoint(0,0);
            stroke(100);
          } else if (lastPoint4.isValid()) {
            plot4.addPoint((float)j,PM10);
            // movimiento del plot
            if (j>50) {
                plot4.moveHorizontalAxesLim(RETRASO);  
            }
          } 
     //}
}


/////////////////////////////////***********/////////////////////////////////
/////////////////////////////////* BOTONES */////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void botonPuertos(){
  fill(#88AF8F);
   stroke(100);
   strokeWeight(1);
   rect(ax*3/100-(ancho/2), ay*3/100-(alto/2), ancho*0.99, alto,5);
   color(100);
   // 2. Dibujar el texto del boton color negro
   fill(0);
   font = loadFont("Arial-Black-48.vlw");
   textFont(font);
   textSize(10);
   text("Configuracion", ax*3/100-(ancho/2)*90/100, ay*3/100+(alto/2)*30/100);
   // verificar si se dio click en el boton
   if(mouseX > ax*3/100-(ancho/2) && mouseX < ax*3/100+(ancho/2) && mouseY > ay*3/100-(alto/2) && mouseY < ay*3/100+(alto/2) && mousePressed==true && pressboton==false && ON_Puertos==false) {
     pressboton = true;
     ON_Puertos = true;
     windowPuertos.background(204,209,209);
     windowPuertos.setVisible(true);
   } 
   if(windowPuertos.isVisible()==false){
     ON_Puertos = false;
     pressboton = false;
   }
   if(ON_Puertos==true){
     COMList.setItems(Serial.list(), 1);
   }
}

void botonInit(){
  if (ON_Datos == false){
    fill(#88AF8F);
  }else{
    fill(#03FC00);
  }
   stroke(100);
   strokeWeight(1);
   rect(ax*6.5/100-(ancho*0.4/2), ay*3/100-(alto/2), ancho*0.4, alto,5);
   color(100);
   // 2. Dibujar el texto del boton color negro
   fill(0);
   font = loadFont("Arial-Black-48.vlw");
   textFont(font);
   textSize(10);
   text("Init", ax*6.5/100-(ancho*0.4/2)*90/100, ay*3/100+(alto/2)*30/100);
   // verificar si se dio click en el boton
   if(mouseX > ax*6.5/100-(ancho*0.4/2) && mouseX < ax*6.5/100+(ancho*0.4/2) && mouseY > ay*3/100-(alto/2) && mouseY < ay*3/100+(alto/2) && mousePressed==true && pressboton==false && ON_Puertos==false && ON_Datos==false) {
     pressboton = true;
     println("presion Init");
     miPuerto.write('A');
     ON_Datos = true;
   } else if(mousePressed == false){
     pressboton = false;
   }
}

void botonStop(){
  if (ON_Datos == true){
    fill(#88AF8F);
  }else{
    fill(#FC0000);
  }
   stroke(100);
   strokeWeight(1);
   rect(ax*8.6/100-(ancho*0.4/2), ay*3/100-(alto/2), ancho*0.4, alto,5);
   color(100);
   // 2. Dibujar el texto del boton color negro
   fill(0);
   font = loadFont("Arial-Black-48.vlw");
   textFont(font);
   textSize(10);
   text("Stop", ax*8.6/100-(ancho*0.4/2)*90/100, ay*3/100+(alto/2)*30/100);
   // verificar si se dio click en el boton
   if(mouseX > ax*8.6/100-(ancho*0.4/2) && mouseX < ax*8.6/100+(ancho*0.4/2) && mouseY > ay*3/100-(alto/2) && mouseY < ay*3/100+(alto/2) && mousePressed==true && pressboton==false && ON_Puertos==false && ON_Datos==true) {
     pressboton = true;
     miPuerto.write('a');
     ON_Datos = false;
     ON_Save = false;
     println("presion Stop");
   } else if(mousePressed == false){
     pressboton = false;
   }
}

void botonSave(){
  if (ON_Save == true && ON_Datos == true){
    fill(#03FC00);
  }else if (ON_Save == false && ON_Datos == true){
    fill(#88AF8F);
  }else{
    fill(#CEC6C6);
  }
  
  stroke(100);
  strokeWeight(1);
  rect(ax*54/100-(ancho*0.4/2), ay*3/100-(alto/2), ancho*0.9, alto*1.1,5);
  color(100);
  // 2. Dibujar el texto del boton color negro
   fill(0);
   font = loadFont("Arial-Black-48.vlw");
   textFont(font);
   textSize(10);
   text("Save Data", ax*54.5/100-(ancho*0.4/2)*90/100, ay*3/100+(alto/2)*32/100);
   if(mouseX > ax*54/100-(ancho*0.4/2) && mouseX < ax*54/100+(ancho*0.9/2) && mouseY > ay*3/100-(alto*1.1/2) && mouseY < ay*3/100+(alto*1.1/2) && mousePressed==true && pressboton==false && ON_Puertos==false && ON_Datos==true && ON_Save==false) {
     pressboton = true;
     miPuerto.write('B');
     ON_Save = true;
     println("presion Save");
   } else if(mousePressed == false && pressboton == true){
     pressboton = false;
   }
}

void botonStopSave(){
  if (ON_Datos == true && ON_Datos == true){
    fill(#88AF8F);
  }else if (ON_Save == false && ON_Datos == true){
    fill(#FC0000);
  }else{
    fill(#CEC6C6);
  }
  
  stroke(100);
  strokeWeight(1);
  rect(ax*54/100-(ancho*0.4/2), ay*7/100-(alto/2), ancho*0.9, alto*1.1,5);
  color(100);
  // 2. Dibujar el texto del boton color negro
   fill(0);
   font = loadFont("Arial-Black-48.vlw");
   textFont(font);
   textSize(10);
   text("Stop Save", ax*54.5/100-(ancho*0.4/2)*90/100, ay*7/100+(alto/2)*32/100);
   if(mouseX > ax*54/100-(ancho*0.4/2) && mouseX < ax*54/100+(ancho*0.9/2) && mouseY > ay*7/100-(alto*1.1/2) && mouseY < ay*7/100+(alto*1.1/2) && mousePressed==true && pressboton==false && ON_Puertos==false && ON_Datos==true && ON_Save==true) {
     pressboton = true;
     miPuerto.write('b');
     ON_Save = false;
     println("presion Stop Save");
   } else if(mousePressed == false && pressboton == true){
     pressboton = false;
   }
}
