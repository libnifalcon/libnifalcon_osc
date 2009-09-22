// create our OSC receiver
OscRecv orec;
OscSend xmit;
xmit.setHost("localhost",9001);
// port 6449
9000 => orec.port;
// start listening (launch thread)
orec.listen();
<<< "TEST" >>>;

    // create an address in the receiver 
    // and store it in a new variable.
    orec.event("/falcon/position, f f f") @=> OscEvent rate_event; 
    
    while ( true )
    { 
        rate_event => now; // wait for events to arrive.
        
        // grab the next message from the queue. 
        while( rate_event.nextMsg() != 0 )
        { 
            // getFloat fetches the expected float
            // as indicated in the type string ",f"
            // buf.play( rate_event.getFloat() );
            // 0 => buf.pos;
            rate_event.getFloat() => float x;
            rate_event.getFloat() => float y;
            rate_event.getFloat() => float z;
            
            //<<< x, y, z >>>;
            if(z < 0.12)
            {
                xmit.startMsg("/falcon/force", "f f f"); 
                0 => xmit.addFloat;
                0 => xmit.addFloat;
                -(z - 0.12) * 10000 => xmit.addFloat;
                //<<< -(z - 0.12) * 2000 >>>;
            }
            else
            {
                xmit.startMsg("/falcon/force", "f f f"); 
                0 => xmit.addFloat;
                0 => xmit.addFloat;
                0 => xmit.addFloat;
            }
        }
    }       
