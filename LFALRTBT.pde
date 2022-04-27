import controlP5.*; //import ControlP5 library
import processing.serial.*;

Serial port;

ControlP5 cp5; //create ControlP5 object
PFont font;

void setup(){ //same as arduino program


  
  printArray(Serial.list());   //prints all available serial ports
  port = new Serial(this, "COM8", 9600);  //You may need to change COMX, where X is the correct OUTGOING port for Arduino!
  //lets add buton to empty window
   size(1000, 400);
  cp5 = new ControlP5(this);
  cp5.addTextfield("Enter Phone Number (10 ch max)").setPosition(20, 33).setSize(200, 40).setAutoClear(false);
  cp5.addTextfield("Enter Message (40 ch max, location will be added to end of message)").setPosition(20, 113).setSize(960, 247).setAutoClear(false);
  cp5.addBang("Submit").setPosition(840, 33).setSize(100, 40);    
  font = createFont("times new roman", 25);    // custom fonts for buttons and title
  cp5.setFont(font);
 
}

//LED off initially, so set bool appropriately
boolean isOpen=false;

void draw(){  //same as loop in arduino

  background(0,0,0); // background color of window (r, g, b) or (0 to 255)
  fill(0, 0, 0);               //text color (r, g, b)
  textFont(font);
}

void Submit() {

  print("the following text was submitted :");
  delay(30);
  
  String number = cp5.get(Textfield.class,"Enter Phone Number (10 ch max)").getText();

  String body = cp5.get(Textfield.class,"Enter Message (40 ch max, location will be added to end of message)").getText();

  String holder= number.concat(body);
  String extraSpace="    ";
  String send= extraSpace.concat(holder);
  //print(" number = " + number);
  //print(" body = " + body);
        char[] sendch = new char[send.length()];
  
        // Copy character by character into array
        for (int i = 0; i < send.length(); i++) {
            sendch[i] = send.charAt(i);
        }
   print(send);

   delay(50);
 for(int i = 0; i < send.length(); i++)
   { 
  port.write(sendch[i]);
delay(50);
   }
  println();
  
}


//define the callback functions for the buttons in GUI
//toggle1 is for button1 
