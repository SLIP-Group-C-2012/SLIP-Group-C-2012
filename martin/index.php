

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Martin Marinov - Group C</title>
<style type="text/css">
<!--
-->
</style>
<link href="../css/main.css" rel="stylesheet" type="text/css" />
<!--[if lte IE 7]>
<style>
.content { margin-right: -1px; } /* this 1px negative margin can be placed on any of the columns in this layout with the same corrective effect. */
ul.nav a { zoom: 1; }  /* the zoom property gives IE the hasLayout trigger it needs to correct extra whiltespace between the links */
</style>
<![endif]-->
<style type="text/css">
<!--
p.MsoNormal {
margin-top:0cm;
margin-right:0cm;
margin-bottom:10.0pt;
margin-left:0cm;
line-height:115%;
font-size:11.0pt;
font-family:"Calibri","sans-serif";
}
-->
</style>
<style type="text/css">
<!--
-->
</style>
<style type="text/css">
<!--
-->
</style>
</head>
<body>

<div class="container">

  <?php include("../includes/header.html"); ?>
  <?php include("../includes/sidebar.html"); ?>

  
  
  <div class="content">
    <h1>Martin Marinov</h1>
    <p>I have worked on several of the different components that were included in the final project. The base idea for a mesh network-based project that relays data to different nodes, was originally proposed by me. It was afterward extended to include audio.</p>
    <p>My main area was the radio but I also designed the audio codec and the different interfaces that the sub teams were supposed to implement. I worked extensively on the integration of the project.</p>
    <h2>Radio</h2>
    <p>The radio chip that the board uses is <a href="http://www.nordicsemi.com/eng/Products/2.4GHz-RF/nRF24L01" target="_blank">nRF24L01</a>. It is a low power radio chip with two main operating modes - transmit mode (TX) and receive mode (RX). The radio cannot handle full-duplex communication therefore it can only transmit when in TX mode and only receive when in RX mode. Switching between the two modes is expensive and should be done as less frequently as possible.</p>
    <p>The layer I developed consists of a simple interface that is used by the rest of the system to send raw packets of data. The idea was to have as simple API as possible with a <em>send</em> and a <em>receive</em> function thus hiding all the complexity inside the interface itself. The goal was to have the radio code handle the radio hardware as efficiently as possible without exposing too much implementation details to the outside world.</p>
    <center><table width="864" border="1">
      <tr>
        <td width="854"><p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define BANDW_1MB (0x0)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define BANDW_2MB (0x8)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define BANDW_250KB (0x20)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define POW_MAX (0x6)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define POW_HIGH (0x4)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define POW_MEDIUM (0x2)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#999999; ">#define POW_LOW (0x0)</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#999988; ">// Use this to handle radio itnerrupts</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <span style="border:none windowtext 1.0pt; padding:0cm; ">radio_handleInterrupt(</span></span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#333333; ">);</span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// Call this once in every interation of the main loop. Services  the radio.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> radio_loop(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">);</span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// Setup the radio at a certain channel (1 - 90),</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// a certain bandwidth from BANDW_1MB, BANDW_2MB or BANDW_250KB</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// and with certain TX power POW_MAX, POW_HIGH, POW_MEDIUM or  POW_LOW</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// if you would like to do hardware acking, set hardwareACK to 1  otherwise set it to 0</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> radio_setup(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> channel, </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> bandwidth, </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> power, </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> hardwareACK);</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// Send a 32 byte packet. data is pointer to the first element of  the packet</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> radio_sendPacket32(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> data);</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;</span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// Receive a 32 byte packet. data is pointer to the first element  of the packet</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// The function will return 1 if data was received and will fill  up the first 32 bytes of the array with it</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
          <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// The function will return 0 if no data was received and will  leave the array untouched</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span></p>
        <p class="MsoNormal" style="margin-bottom:.0001pt;line-height:12.0pt;"><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> radio_receivePacket32(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> data);</span></p></td>
      </tr>
    </table></center>
    <p><em><br />
    radio_setup()</em> sets up the radio hardware to a given channel and allows for some additional setup.</p>
    <p><em>radio_loop()</em> manages the state of the radio and thus it must be called on each main loop iteration. It fetches information from radio interrupts.</p>
    <p><em>radio_sendPacket32()</em> tries to send a packet via the radio interface. If the radio is not in TX mode, it will switch it to TX mode (unless the radio is busy). The radio does not allow to send multiple packets too fast, therefore this method blocks until an interrupt is received from the radio that the previous packet was successfully sent. There is a mechanism that prevents deadlocks. Correct handling of packets that are being sent is vital since the radio can become in an inconsistent state if the radio has not finished its previous task.</p>
    <p><em>radio_receivePacket32()</em> tries to receive a packet from the radio interface. It will switch to RX mode if the radio is not busy. </p>
    <p>The result of using the <em>radio.c</em> interface is that sending multiple packets would require only one switch to TX mode. The same goes for receiving packets. This ensures that the radio spends as less time as possible switching modes.</p>
    <h3>Results:</h3>
    <ul>
      <li>We were successfully able to transmit real time audio with speed of <strong>62.5</strong> kbits/s. The system showed that it could handle this sufficiently well and that there was even room for higher speeds. Since we didn't need higher rates than that, we didn't test for higher speeds but it is definitely possible.</li>
      <li>We also noticed that it is impossible to do real time retransmission of the audio data. This is a limitation of the radio hardware itself due to the big delay that is needed for the radio to switch modes. Sending data at rate 8000 B/s means that we are transmitting 250 packets/s. Therefore in order to have real time audio, a packet must not take more than 0.004 s. We were unable to make the radio receive a packet, switch from RX mode to TX mode, retransmit it and go back into RX mode in under 0.004 s therefore we were not able to do real-time mesh networking retransmissions with audio.</li>
      <li>When sending with this rate, even though hardware CRC was enabled, we were receiving a large number of scrambled packets when the signal was weak. The solution we came up with is to use advanced CRC for text messages and tolerate scrambled data for audio transmissions.</li>
    </ul>
    <h2>Serial input
    </h2>
    <p>I developed a small library for asynchronously handling input over the serial cable from the computer.</p>
    <center>
    <table width="864" border="1">
      <tr>
        <td width="854"><pre style="line-height:12.0pt;"><em><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#999988; ">// if there is data pending, it will fill the data array</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// with as much data is available</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333;"> Z<br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// the return value is the number of bytes written to the data array</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// data should be 32 byte in size</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// if computer has sent more than 32 bytes after last call, the extra data has been dropped</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><strong><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#445588; ">int</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <span style="border:none windowtext 1.0pt; padding:0cm; ">serial_receiveData</span>(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> data);</span></pre>
          <pre style="line-height:12.0pt;"><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// returns a string if the computer has sent one</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// returns 0 if no string is available</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// returns 1 if there is a string and it has been copied to the str array</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// Don't use this if you expect data rather than a string, otherwise this will scramble the data</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">// The strings that the PC sends must be below 32 bytes</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">int</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> serial_getString(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">uint8_t</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> str);</span></pre></td>
      </tr>
    </table> </center>
    <p><br />
    It uses a UART interrupt to handle incoming data bits from the PC and store them into an internal buffer. Whenever <i>serial_getString()</i> is called if the buffer contains a line terminating character, a string is returned to the caller thus proving the asynchronous behavior for sending commands to the device as demonstrated in the final presentation. This ability was vital for debugging.</p>
    <h2>Codec</h2>
    <p>The audio codec was the first thing that was actually ready for the project. I tested it thoroughly on a Windows based PC. We tried integrating it into the boards and it was working correctly when the data was being compressed/uncompressed on the same device. As soon as we tried using the radio, the scrambled packets that were being received were making it unusable. We didn't have time to look into this issue since there were important integration problems that needed to be dealt with and we did already measure that the radio speed will be sufficient to send uncompressed audio.</p>
    <p>Nevertheless, I spend a lot of time designing the codec and therefore would like to discuss how it works. It is based on differences between successive sample values. Those differences are based on a dictionary that is approximating the steps. The dictionary could have size of 2, 4, 8, 16, 32, 64 and 128 and therefore each 8bit sample could be compressed to 1bit, 2bits, 3bits, 4bits, 5bits, 6bits or 7bits. The dictionary is the same on the encoder and decoder side. Every packet also contains 1 synchronization byte wich recovers the stream in case of packet loss. The interface is:</p>
    <center>
    <table width="864" border="1">
      <tr>
        <td width="854"><pre style="line-height:12.0pt;"><em><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#999988; ">/* Initializes the compression parameters.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "><br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">    src_packet_size - the size of the packet that would be offered as a src to compress</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">    and will be returned as dst to uncompress. Should be a power of 2.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;<br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">    compression_packet_size - the required size of the packet after compression. Should be less than src_packet_size.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">    The value should be a multiple of 2 (with some limitations) + 1. There is one sync bit used to enable loss tolerance.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; "></span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; "> */</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><strong><span style="border:none windowtext 1.0pt; padding:0cm; font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <span style="border:none windowtext 1.0pt; padding:0cm; ">set_up_compression</span>(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">int</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> src_packet_size, </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">int</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> compressed_packet_size);<br /></span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;<br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">/* Expects src array of PCM data of length specified by src_packet_size during set_up_compression.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">Returns dst array of compressed data of length compressed_packet_size specified during set_up_compression.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">The resulting array could be stored/transmitted to be decoded via uncompress back to PCM data */</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> compress(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">unsigned</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">char</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> src, </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">unsigned</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">char</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> dst);<br /></span><span style="font-family:Consolas; font-size:9.0pt; color:#333333; ">&nbsp;<br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">/* Expects src array of compressed data of length specified by compressed_packet_size during set_up_compression.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">Returns dst array of PCM data of length src_packet_size also specified during set_up_compression.</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><em><span style="font-family:Consolas; font-size:9.0pt; color:#999988; ">The resulting array could be played back directly as it is. */</span></em><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <br /></span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">void</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> uncompress(</span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">unsigned</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">char</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> src, </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">unsigned</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> </span><strong><span style="font-family:Consolas; font-size:9.0pt; color:#445588; ">char</span></strong><span style="font-family:Consolas; font-size:9.0pt; color:#333333; "> <strong>*</strong> dst);</span></pre></td>
      </tr>
    </table></center>
    <p><br />
    The <em>set_up_compression()</em> method creates the dictionary according to the compression size required. For example if we have a source package of 64bytes and a compressed packet size of 25bytes then we have compressed every 8bit sample into 3bits (keep in mind we have an extra synchronization byte into the compressed packet). Therefore we have introduced a 2.56x compression. Below you could find several examples of waves of the same input PCM at different compression levels.</p>
    
    <center><p>
    <b>Uncompressed</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=uncompressed.mp3" />
</object>
</p></center>

    <center><p>
    <b>7bits per sample - 1.12x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=7bits1_12.mp3" />
</object>
</p></center>

    <center><p>
    <b>6bits per sample - 1.31x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=6bits1_31.mp3" />
</object>
</p></center>

    <center><p>
    <b>5bits per sample - 1.56x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=5bits1_56.mp3" />
</object>
</p></center>

    <center><p>
    <b>4bits per sample - 1.94x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=4bits1_94.mp3" />
</object>
</p></center>

    <center><p>
    <b>3bits per sample - 2.56x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=3bits2_56.mp3" />
</object>
</p></center>

    <center><p>
    <b>2bits per sample - 3.76x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=2bits3_76.mp3" />
</object>
</p></center>

    <center><p>
    <b>1bit per sample - 7.11x</b><br/>
<object type="application/x-shockwave-flash" data="player_mp3_mini.swf" width="200" height="20">
    <param name="movie" value="player_mp3_mini.swf" />
    <param name="bgcolor" value="#888888" />
    <param name="FlashVars" value="mp3=1bit7_11.mp3" />
</object>
</p></center>

    <h2>Routing</h2>
    <p>I worked with Henrik and Iliyan on the routing protocol. We tried several implementations before coming up with the final retransmission based protocol. The protocol is quite simple and is presented as a wrapper around the radio interface.  It supports text and audio mode and implements a custom CRC for the text mode. In text mode the protocol implements a flood-based mesh network in which every device retransmits every packet it receives. Each packet has an id. We assume that no two devices are transmitting at the same time (real walkie-talkie systems rely on the same assumption). Therefore, each device does the following:</p>
    <ul>
      <li>Keeps track of last received packet id - <em>pack_id</em></li>
      <li>When a new packet <em>packet</em> is received, if<em> packet.id</em> &gt;<em> pack_id</em>, the packet is propagated to the API output and retransmitted automatically, then <em>pack_id</em> = <em>packet.id</em>. Otherwise the packet is rejected.</li>
      <li>If we are really out of sync with the network, we can accept a packet even if it's id is smaller than <em>pack_id</em>. This will only happen when the id is significantly smaller. This also deals with counter overflows.</li>
    </ul>
    <h2><strong>Integration</strong></h2>
    <p>My other major contribution towards the project was to guide the different teams towards a common interface that could afterwards fit together. The integration took most of my time towards the end of the SLIP project. Unfortunately I can't pinpoint any particular major contribution I have made during this time, I mainly was ensuring that people were working on their parts of the project. I have a good knowledge of all of the APIs that were introduced including some of the implementation details of the different components. I made sure that the different components were compatible and built the <em>main</em> method in <em>main.c</em>. I spent approximately 40% of the time I worked on SLIP onto the integration part.</p>
    <h2>Conclusion</h2>
    <p>Although we weren't able to completely cover all of the desired features we wanted to have, I believe in the end we had a good and working prototype that was able to demonstrate audio transmission and mesh networking (although not necessarily at the same time). The system is just a prototype but could be easily adapted to a consumer-grade device (with a slight change of the hardware). I believe that our project was successful and can serve as a good proof-of-concept.</p>
  </div>
  
  
  <?php include("../includes/footer.html"); ?>
