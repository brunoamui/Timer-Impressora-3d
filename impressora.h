

class Impressora
{
	// Class Member Variables
	// These are initialized at startup
	unsigned char TAG_SELF[4];// The UUID of the tag
        Adafruit_PCD8544* display_self;
        unsigned char name_self[16];
        Adafruit_Thermal* printer_self;

	// These maintain the current state
        unsigned int Last = 0;
        unsigned char On = 0;
        unsigned int delta;
        unsigned int sumUUID = 0;
        unsigned char printer_configured = 0;
        
        public:
        Impressora(Adafruit_PCD8544* display,const char TAG[4], const char* _name)
        {
          display_self = display;
          for(int i = 0; i<4; i++)
          {
            TAG_SELF[i] = TAG[i];
          }
          for(int i = 0; i<16; i++)
          {
            name_self[i] = _name[i];
          }
        }
        
        Impressora(Adafruit_PCD8544* display,const char TAG[4], const char* _name, Adafruit_Thermal* printer)
        {
          display_self = display;
          printer_self = printer;
          printer_configured = 1;
          for(int i = 0; i<4; i++)
          {
            TAG_SELF[i] = TAG[i];
          }
          for(int i = 0; i<16; i++)
          {
            name_self[i] = _name[i];
          }
        }
        
        void ImprimeCoupon(unsigned int delta)
        {
          printer_self->sleep();      // Tell printer to sleep
          printer_self->wake();       // MUST call wake() before printing again, even if reset
          printer_self->setDefault(); // Restore printer to defaults
        
          printer_self->justify('C');
          printer_self->doubleHeightOn();
          printer_self->println("Nota de uso Impressora 3d");
          printer_self->doubleHeightOff();
          
          printer_self->feed(2);
          
          printer_self->justify('L');
          printer_self->underlineOn(); 
          printer_self->print("Impressora Utilizada:");
          printer_self->underlineOff();
         
          
          for(int i=0; i<16; i++)
          {
             if(name_self[i]>=57 && name_self[i]<=122)
             {
               printer_self->print((char)name_self[i]);
             }
          }
          printer_self->println(" ");
          
          printer_self->underlineOn(); 
          printer_self->print("Tempo Utilizado:");
          printer_self->underlineOff();
          printer_self->print((::day(delta)-1)*24 + ::hour(delta));
          printer_self->print("h");
          printer_self->print(::minute(delta));
          printer_self->print("m");
          printer_self->print(::second(delta));
          printer_self->print("s");
          
          
          printer_self->feed(2);
          
          // Print the 75x75 pixel logo in adalogo.h
          printer_self->printBitmap(fablogo_width, fablogo_height, fablogo_data);
        
          printer_self->feed(3);
        
          printer_self->sleep();      // Tell printer to sleep
          printer_self->wake();       // MUST call wake() before printing again, even if reset
          printer_self->setDefault(); // Restore printer to defaults
          Serial.println("ImprimeCoupon OUT");
        }
        
        unsigned int compareUUID(unsigned char* readTag)
        { 
          sumUUID = 0;
          
          for (int i = 0; i < 4; i++)
          {
           sumUUID+= readTag[i] - TAG_SELF[i]; 
          }  
          
          if(sumUUID == 0)
          {
            return 1;
          }else
          {
            return 0;
          }
        }
        
        void Clear()
        {
          display_self->clearDisplay();   //Apaga o buffer e o display
          display_self->setCursor(0,0);  //Seta a posição do cursor
        }
        
        void Update()
        {
          display_self->display();
        }
        
        void Print_name()
        {
          for(int i=0; i<16; i++)
          {
             if(name_self[i]>=57 && name_self[i]<=122)
             {
               display_self->print((char)name_self[i]);
             }
          }
          display_self->print("\n");
        }
        
        void Check_tag(unsigned char* readTag)
        {
          if(compareUUID(readTag))
          {
            Clear();
            Print_name();
            if(On!=0)
            {
              On=0;
              delta = ::now() - Last;
              display_self->println(F("DESLIGANDO"));
              display_self->print(F("TEMPO LIGADO: "));
              display_self->print((::day(delta)-1)*24 + ::hour(delta));
              display_self->print("h");
              display_self->print(::minute(delta));
              display_self->print("m");
              display_self->print(::second(delta));
              display_self->print("s");
              
              if(printer_configured!=0)
              {
                ImprimeCoupon(delta);
              }
            }
            else
            {
              On=1;
              display_self->println(F("LIGANDO"));
              Last = ::now();
              display_self->display();
            }
            Update();
          }
          
        }
        
};
