#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <FreeDefaultFonts.h>


/* ASCII control characters recognised
*/
#define CR	'\r'
#define LF	'\n'
#define VT	'\v'
#define BS	'\b'
#define FF	'\f'
#define ESC	'\e'
#define NUL '\0'

// standard opening escape sequences
#define esc_ctrl '^['
#define esc_octal '\033'
#define esc_hex '\x1b'
#define esc_unicode '\u001b'
#define esc_dec 27


#define ctrlseq_nfields	2		// Max. no of numeric fields (separated by ';') in a control sequence


#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define BLUE    0x001F
#define MAGENTA 0xF81F
#define CYAN    0x07FF
#define WHITE   0xFFFF
#define BRIGHT_BLACK  0x0800
#define BRIGHT_RED    0xF800
#define BRIGHT_GREEN  0x07E0
#define BRIGHT_YELLOW 0xFFE0
#define BRIGHT_BLUE   0x001F
#define BRIGHT_MAGENTA 0xF81F
#define BRIGHT_CYAN    0x07FF
#define BRIGHT_WHITE   0xFFFF

int current_fg = WHITE;
int current_bg = BLACK;

/* Timed processing
*/
unsigned long then;
bool cursor_on=true;
int cursor_delay=300;

/* Control sequence processing
*/
int ctrlseq_p[ctrlseq_nfields];
int ctrlseq_pos;



/* setup() - standard Arduino "Init Task"
*/
void setup(void)
{

  // init computer serial
  Serial.begin(9600);

  // init tft display
  uint16_t ID = tft.readID();
  // Serial.print("found ID = 0x");
  // Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextColor(current_fg, current_bg);
  tft.setTextSize(1);
  tft.setTextWrap(false);
	ctrlseq_pos = -1;					// Initialise control sequence processing
  then = millis();						// Initialise blink
}

/* loop() - standard Arduino "Background Task"
*/
void loop(void)
{

  unsigned long now = millis();
  if (now - then > cursor_delay) {
    then = now;
    cursor_on = !cursor_on;
    if(cursor_on) {
      tft.print("_");
      tft.setCursor(tft.getCursorX()-6, tft.getCursorY());
    } else {
      tft.print(" ");
      tft.setCursor(tft.getCursorX()-6, tft.getCursorY());
    }
  }

  if (Serial.available()){
    char c = Serial.read();
    Serial.write(c);
    if ( c >= 0 )
    {
      if (ctrlseq_pos<0){//we are not currently considering an escaope sequence
        // if (c==esc_unicode){
        //   ctrlseq_pos = 0;
        // }else if(c==CR){
        //   tft.setCursor(0, tft.getCursorY());
        // }else{
        //   tft.print(c);
        // }
        switch(c){
          case CR:
            tft.setCursor(0, tft.getCursorY());
            break;
          case LF:
          {
            int y=tft.getCursorY();
            // tft.setCursor(0, tft.getCursorY()+10);
            tft.fillRect(0, y+10, tft.width(), 10, BLACK); // sensures blank line
            tft.setCursor(0, y+10);
          }

            break;
          case VT:
            tft.setCursor(0, tft.getCursorY()-10);
            break;
          case BS:
            {
              int x = tft.getCursorX();
              int y = tft.getCursorY();
              tft.print(" ");
              tft.setCursor(x-6, y);
              tft.print(" ");
              tft.setCursor(x-6, y);
            }
            break;
          case FF:
            tft.fillScreen(BLACK);
            tft.setCursor(0, 0);
            break;
          case esc_unicode:
            ctrlseq_pos = 0;
            break;
          default:
            int y = tft.getCursorY();
            int x = tft.getCursorX();
            if (y>=tft.height()-10){
              // Serial.print("3SCROLLING");
              // tft.vertScroll(6, 10, 0);
              tft.setCursor(x, 0);
            }
            tft.print(c);
            break;
        }
      } else if (ctrlseq_pos>=0 && ctrlseq_pos<ctrlseq_nfields){
        if (c>='0' && c<='9'){
          ctrlseq_p[ctrlseq_pos] = ctrlseq_p[ctrlseq_pos]*10 + (c-'0'); // why - '0'?? //TODO: check this
        }else if (c==';' ){
          ctrlseq_pos++;
          
        }else if (c=='['){
        }else if(c>0x40 && c<0x7E){
          // deal with the control sequence
          switch(c){
            case 'm':
              if( ctrlseq_pos==1){
                if (ctrlseq_p[0]==0){
                  switch(ctrlseq_p[1]){
                    case 0:
                      current_fg = WHITE;
                      current_bg = BLACK;
                      break;
                    case 30:
                      current_fg = BLACK;
                      break;
                    case 31:
                      current_fg = RED;
                      break;
                    case 32:
                      current_fg = GREEN;
                      break;
                    case 33:
                      current_fg = YELLOW;
                      break;
                    case 34:
                      current_fg = BLUE;
                      break;
                    case 35:
                      current_fg = MAGENTA;
                      break;
                    case 36:
                      current_fg = CYAN;
                      break;
                    case 37:
                      current_fg = WHITE;
                      break;
                    case 40:
                      current_bg = BLACK;
                      break;
                    case 41:
                      current_bg = RED;
                      break;
                    case 42:
                      current_bg = GREEN;
                      break;
                    case 43:
                      current_bg = YELLOW;
                      break;
                    case 44:
                      current_bg = BLUE;
                      break;
                    case 45:
                      current_bg = MAGENTA;
                      break;
                    case 46:
                      current_bg = CYAN;
                      break;
                    case 47:
                      current_bg = WHITE;
                      break;
                    default:
                      Serial.print("uc");
                      Serial.print(ctrlseq_p[0]);
                      break;
                  }
                } else{
                  switch(ctrlseq_p[1]){
                    case 0:
                      current_fg = WHITE;
                      current_bg = BLACK;
                      break;
                    case 30:
                      current_fg = BRIGHT_BLACK;
                      break;
                    case 31:
                      current_fg = BRIGHT_RED;
                      break;
                    case 32:
                      current_fg = BRIGHT_GREEN;
                      break;
                    case 33:
                      current_fg = BRIGHT_YELLOW;
                      break;
                    case 34:
                      current_fg = BRIGHT_BLUE;
                      break;
                    case 35:
                      current_fg = BRIGHT_MAGENTA;
                      break;
                    case 36:
                      current_fg = BRIGHT_CYAN;
                      break;
                    case 37:
                      current_fg = BRIGHT_WHITE;
                      break;
                    case 40:
                      current_bg = BRIGHT_BLACK;
                      break;
                    case 41:
                      current_bg = BRIGHT_RED;
                      break;
                    case 42:
                      current_bg = BRIGHT_GREEN;
                      break;
                    case 43:
                      current_bg = BRIGHT_YELLOW;
                      break;
                    case 44:
                      current_bg = BRIGHT_BLUE;
                      break;
                    case 45:
                      current_bg = BRIGHT_MAGENTA;
                      break;
                    case 46:
                      current_bg = BRIGHT_CYAN;
                      break;
                    case 47:
                      current_bg = BRIGHT_WHITE;
                      break;
                    default:
                      Serial.print("uc");
                      Serial.print(ctrlseq_p[0]);
                      break;
                  }
                }
              } else if (ctrlseq_pos == 0){
                switch (ctrlseq_p[0])
                {
                case 0:
                  current_fg = WHITE;
                  current_bg = BLACK;
                  break;
                default:
                  break;
                }
              }
              tft.setTextColor(current_fg, current_bg);
            break;
            case 'A':
              tft.setCursor(tft.getCursorX(), tft.getCursorY()-ctrlseq_p[0]*10);
            break;
            case 'B':
              tft.setCursor(tft.getCursorX(), tft.getCursorY()+ctrlseq_p[0]*10);
            break;
            case 'C':
              tft.setCursor(tft.getCursorX()+6*(int)ctrlseq_p[0], tft.getCursorY());
            break;
            case 'D':
              tft.setCursor(tft.getCursorX()-6*(int)ctrlseq_p[0], tft.getCursorY());
            break;
            case 'S':
              tft.vertScroll(tft.getCursorX(), tft.getCursorY(), ctrlseq_p[0]*10);
            break;
            case 'T':
              tft.vertScroll(tft.getCursorX(), tft.getCursorY(), -ctrlseq_p[0]*10);
            break;
            default:
              Serial.print(" (ucs");
              Serial.print(c);
              Serial.print(") ");
              break;
          }
          ctrlseq_pos = -1;
          ctrlseq_p[0]=0;
          ctrlseq_p[1]=0;
        }
    }
  }
}
}

