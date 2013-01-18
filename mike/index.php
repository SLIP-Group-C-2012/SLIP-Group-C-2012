<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Group C</title>
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
hr {
  width: 90%;
  color: #666;
  background-color: #666;
  margin-top: 20px;
  margin-bottom: 20px;
}
.content img {
  display: block;
  margin-left: auto;
  margin-right: auto;
  text-align: center;
  padding-bottom: 20px;
}
h3 {
  text-decoration: underline;
}
</style>
</head>
<body>

<div class="container">

  <?php include("../includes/header.html"); ?>
  <?php include("../includes/sidebar.html"); ?>

  
  
  <div class="content">
    <h1>Mike Walters - s0806653</h1>
    <h2>Introduction</h2>
    <p>My main focus on the project was on the audio input; creating an amplifier and configuring the processor to sample the audio data at the appropriate resolution and rate. I then worked with Richard to integrate this with the DMA controller code he worked on.</p>
    <p>I later investigated alternative means of audio output, since our original plan to use an I<sup>2</sup>S digital-to-analog converter proved to be impossible.</p>
    <p>I should note that, aside from A-Level phsyics, I started this project with no electronics knowledge. I learned about all of the sampling theory, basic electronics and use of the test equipment that is shown below during the course of this project.</p>
    
    <h2>Audio Input</h2>
    <h3>Requirements</h3>
    <p>Our project is primarily aimed towards use in disaster recovery situations where there is minimal infrastructure and a quick and easy communication system is required. This use-case leads to the following requirements:</p>
    <ul>
      <li>Record speech with reasonable quality and minimal interference. Clarity is important for potential disaster recovery uses.</li>
      <li>Minimise audio bandwidth to allow for the mesh network to scale well.</li>
    </ul>
    
    <h3>Design Specification</h3>

    <h4>ADC</h4>
    <p>The Prospeckz has an onboard ADC (analog to digital converter) which is what we use to read the analog signal from the microphone. Since we're only looking to transmit speech and we need to limit the bandwidth as much as possible, we decided to use a sampling rate of 8KHz and a resolution of 8bits per sample. This gives us adequate resolution and frequency to reproduce human speech clearly, while keeping the bandwidth very low.</p>
    <p>The processer does have configurable opamps but, since they cannot be configured appropriately for our needs, they should be configured to pass through the signal unchanged (ie: unity gain or gain of 1).</p>

    <h4>Amplifier</h4>
    <p>Since the on-board ADC can only convert a signal between 0 and 2.2V, we needed an external amplifier that would serve two purposes:</p>
    <ul>
      <li>Amplify the low amplitude signal from the microphone so we make full use of the range of the ADC</li>
      <li>Offset the signal so that it was centered around 1.1V rather than 0V, since the ADC cannot read negative voltages</li>
    </ul>
    
    <h3>Implementation</h3>
    <p>Configuration of the ADC and opamps was trivial, since there are extensive <a href="http://www.energymicro.com/downloads/application-notes">application notes provided by EnergyMicro</a>. The basic steps were:</p>
    <ul>
      <li>Enable the ADC and DAC clocks</li>
      <li>Configure the appropriate opamp to output to the ADC and have a gain of 1</li>
      <li>Enable the opamp</li>
    </ul>
    <p>I then created a simple test program that used the processor to query the ADC directly, convert values and output them over serial. Voltages could then be applied to the input pin and seen on the serial output.</p>

    <p>I then designed a circuit around a standard differential amplifier example, with variable gain so we could tune it later.</p>
    <img src="images/amp-circuit-v1.png" />
    <p>I built it up on a breadboard for testing and later made copies soldered onto stripboard to be more reliable for the demo.</p>
    <img src="images/amp-breadboard.jpg" />
    <img src="images/amp-stripboard.jpg" />
    <p>After connecting it up and tuning the gain, we could record some audio samples!</p>
    <p>I had some initial problems with excess noise in the samples and narrowed it down to noise in the power supply being amplified and added to the output signal. The output from the amplifier with no sound input is shown below, it shows around 0.2V ripple which is fairly significant on our range.</p>
    <img src="images/power-ripple.jpg" />
    <p>The circuit was refined slightly to add some bulk capacitance (~100uF, though more would be better) across the power rails. Also a 4KHz low-pass filter was added just before the connection to the ADC to attenuate any frequencies that we could not reproduce with our sampling rate (more details of this are shown later in the report). The improvement is shown below.</p>
    <img src="images/power-smooth.jpg" />
    
    <h3>Conclusion</h3>
    <p>I think this part of the project was a success. It met the requirements and we could record and transmit speech from one device to another. Given more time I would like to investigate more options for removing all noise, possibly doing filtering in software. I would also like to do more testing in different environments to see if interference could cause any problems.</p>
    
    <hr />

    <h2>Low-pass Filtering</h2>
    <p>Since low-pass filters are quite important in several places in our project, I decided to design and test a simple RC filter that could be reused throughout the project. Also, since I don't have any electronics background, I wanted to convince myself that the theory and my understanding of the formulae were correct.</p>
    <p>For our sampling rate of 8KHz, according to the Nyquist-Shanon sampling theorem, we can reproduce frequencies up to 4KHz. So on the input and output we want to filter out any frequencies above that since they can lead to strange artifacts. Using an <a href="http://sim.okawa-denshi.jp/en/CRlowkeisan.htm">online RC-filter calculator</a>, I came up with the following filter using a 4.4KOhm resistor and a 10nF capacitor, with a theoretical -3dB attenuation at 4KHz:</p>
    <img src="images/lowpass-circuit.png" />
    <p>To test the filter I set up an arbitrary function generator to output a sine wave sweep from 0 - 30KHz and connected it through the RC filter to an oscilloscope. The function generator also outputs a sawtooth wave representing the frequency of the sweep, I connected this to the second oscilloscope channel and used it to trigger off.</p>
    <img src="images/lowpass-diagram.png" />
    <p>With some adjustment of the oscilloscope I was able to display the positive half of the sweep full screen on the display. In this configuration, you can treat the vertical axis on the oscilloscope as amplitude and the horizontal axis as frequency</p>
    <img src="images/lowpass.jpg" />
    <p>With some annotations you can see that it gives the expected behaviour with around -3dB at 4KHz. Obviously this test setup isn't the most accurate, but it gives a fairly good indication that the filter is working correctly.</p>
    <img src="images/lowpass-annotated.jpg" />
    
    <hr />
    
    <h2>I<sup>2</sup>C DAC</h2>
    <p>Our original plan for audio output was to use PWM initially and eventually move to an I<sup>2</sup>S DAC (digital-to-analog converter). We found out quite late in the project that there were not enough correctly configured and available UARTs to do this, so I started to investigated an alternative.<p>
    <p>I found a 12-bit DAC (the MCP4725 from Microchip) which could operate over I<sup>2</sup>C. However, I<sup>2</sup>C is not designed for audio so it may have been too slow to keep up with the bandwidth, but I figured it was worth experimenting.<p>
    
    <h3>Breakout board</h3>
    <p>Since the DAC comes in a small SOT-23 package, I needed a breakout board to allow me to easily test it with a breadboard. I designed a basic breakout board shown below and had it milled out from copper-clad sheet.</p>
    <img src="images/i2c-dac-design.jpg" />
    
    <h3>Testing</h3>
    <p>Initial tests of the DAC lead to either nothing happening or very erratic behaviour in the output. I traced the I<sup>2</sup>C signals using a logic analyser and identified that some parts of the waveform did not match up correctly to what was being sent.</p>
    <img src="images/logic-analyser.jpg" />
    <p>Since I did not know whether the problem lay in my code, the connections or with the DAC itself, I wanted to limit the problem space. I tested with an Arduino since it has a very simple library and has been very well tested by many beginners. I was able to get the DAC working and outputting a sine wave.</p>
    <img src="images/dac-sine.jpg" />
    <p>To see if this DAC would be viable for audio output, I set the Arduino to run the I<sup>2</sup>C bus at 400Kbit (fast mode) and used the fast mode on the DAC (which only uses 2 bytes for data, plus the address byte). I set the oscilloscope to 50 microseconds per division and measured the time spent on each sample to get an idea of the sampling rate. As shown below, each sample was about 125 microseconds wide, which leads to an 8KHz sample rate. Perfect!</p>
    <img src="images/dac-zoom.jpg" />

    
    <h3>Conclusion</h3>
    <p>Unfortunately I was not able to get the DAC working with the Prospeckz in time. I suspect that the problem was due to voltage level differences between the supported voltages in the DAC and the Prospeckz' operating voltage. A standard voltage level converter would be needed to get the two working together - with this I am confident that I would be able to use this DAC for audio output and improve the audio quality over the current PWM output.</p>
  </div>
  
  
  <?php include("../includes/footer.html"); ?>
