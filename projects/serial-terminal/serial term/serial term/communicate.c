 unsigned char key;
int comunicate (void)

{
       key=getchar_kb();
       if(key==0xfd)
       return 0;
       else
       
       if(key==0xff || key == 0xfe)
        do_states(key);
       else
       {
       if(key!=0 )
       {
        //putchar(key);
        //key=getchar();
        if(key==13)
        {
                //putchar(13);
                putchar(10);
        }
        else 
        
        putchar(key);       
        //do_states(key);
       }
       } 
       //key=getchar();
       //if(key!=1)
       //{
       
       key=getchar();
       if(key==0x1b)
              {        
                //while (key!=0x6d)
                //{
                  //      key=getchar();
                //}
       }
       else
       {
               if(key!=0 && key!=0xff && key!=0x0fe)
               //printf("%c",key);
               {
               do_states(key);
               }
       }  
       
       return 1;
    }