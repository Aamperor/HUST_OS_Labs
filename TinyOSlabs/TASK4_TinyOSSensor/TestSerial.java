/*									tab:4
 * Copyright (c) 2005 The Regents of the University  of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the copyright holders nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * Java-side application for testing serial port communication.
 *
 *
 * @author Phil Levis <pal@cs.berkeley.edu>
 * @date August 12 2005
 */

import java.io.IOException;

import net.tinyos.message.*;
import net.tinyos.packet.*;
import net.tinyos.util.*;

public class TestSerial implements MessageListener 
{
  private MoteIF moteIF;

  public TestSerial(MoteIF moteIF) 
  {
    this.moteIF = moteIF;
    this.moteIF.registerListener(new SenseMsg(), this);
  }

  public void sendPackets() 
  {
    int counter = 0;
    SenseMsg payload = new SenseMsg();

    try 
    {
      while (true) 
      {
        System.out.println("Sending packet " + counter);
        payload.set_counter(counter);
        moteIF.send(0, payload);
        counter++;
        try 
        {
          Thread.sleep(1000);
        } 
        catch (InterruptedException exception) 
        {
        }
      }
    } 
    catch (IOException exception) 
    {
      System.err.println("Exception thrown when sending packets. Exiting.");
      System.err.println(exception);
    }
  }

  public void messageReceived(int to, Message message) 
  {
    SenseMsg msg = (SenseMsg)message;
    int type = msg.get_nodeid();
    double tempature;
    double humidity;
    double photo;
    switch (type) 
    {
      case 1:
        tempature = -40.1 + 0.01 * msg.get_counter();
        System.out.printf("received temperature:%.2f", tempature);
        System.out.println("℃C");
        break;
      case 2:
        humidity = -4 + 0.0405 * msg.get_counter() +
                  (-2.8 / 1000000) * msg.get_counter() * msg.get_counter();
        System.out.printf("received humidity:%.2f", humidity);
        System.out.println("%");
        break;
      case 3:
        photo = msg.get_counter() * 1.5 / 4096 / 10000;
        photo = 0.625 * 1000000 * photo * 1000;
        System.out.printf("received photo:%.2f", photo);
        System.out.println("Lux");
        System.out.println();
        break;
      default:
        System.out.println("received unknow data:" + msg.get_counter());
        break;
    }
  }

  private static void usage() 
  {
    System.err.println("usage: TestSerial [-comm <source>]");
  }

  public static void main(String[] args) throws Exception 
  {
    String source = null;
    if (args.length == 2) 
    {
      if (!args[0].equals("-comm")) 
      {
        usage();
        System.exit(1);
      }
      source = args[1];
    } 
    else if (args.length != 0) 
    {
      usage();
      System.exit(1);
    }

    PhoenixSource phoenix;

    if (source == null) 
    {
      phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
    } 
    else 
    {
      phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
    }

    MoteIF mif = new MoteIF(phoenix);
    TestSerial serial = new TestSerial(mif);
    serial.sendPackets();
  }
}
