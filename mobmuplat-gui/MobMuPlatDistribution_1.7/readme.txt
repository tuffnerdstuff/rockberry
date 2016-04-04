README for MobMuPlat distro version 1.68 - 27 May 2015

see wwww.mobmuplat.com for the below and more

MobMuPlat (short for Mobile Music Platform) is a set of tools for creating music apps for iOS and Android devices. See www.mobmuplat.com

MobMuPlat is a standalone iOS/Android app which hosts and loads from a list of available works. Creating your own work consists of two parts. First, creating a graphical user interface (GUI) with the MobMuPlat Editor; second, creating the audio engine using the graphical programming language Pure Data (PD). With both of these applications open, and data sent between the two, you can simulate the app behavior on your laptop/desktop. Once development is complete, just drag the two saved files into the "Documents" folder of iTunes, and they are uploaded to your device and can be opened in the MobMuPlat app.

MobMuPlat can do synthesis, sampling, networking via local wifi, query and set hardware characteristics, display images/graphical scores, and much more.

====SETUP
1. Put the MobMuPlat app on your iOS device. MobMuPlat is still in development and not on the app store; it is currently distributed via TestFlight, and app for distributing apps for beta testing. Contact me if you are not on my distribution list for it. 
2. Download and unpack the development package. It contains the MobMuPlat Editor, a pair of utility PD files, and a folder of tutorial patch files
3. Get PureData vanilla .46: http://msp.ucsd.edu/software.html. (.46 is necessary for communication with the desktop editor via OSC. However, patches made on previous versions will still load correctly in the app). 
4. Learn how to program in PD. That takes a bit of time. Some places to start: 
http://en.flossmanuals.net/pure-data/
http://www.schoox.com/4734/pure-data-tutorials-by-rafael-hernandez#.UPn7K7TpYrQ 
http://puredata.info/docs/tutorials

===CROSS-PLATFORM NOTES
For OSX 10.7 and above use the Cocoa version of the editor
For OSX 10.6 and below use the Java version of the editor

For Windows: use the Java version of the editor.
Windows notes:
0) Make sure you have Java installed, so that you can double click on the editor .jar file to start it.
1) Networking: Windows has a hard time accepting or sending multicast UDP messages, and so networking (like in MMPTutorial4-Networking) may not work
2) Fonts: The font chooser contains available iOS fonts, most of which are not usually installed on Windows systems. It will not render as the selected font, but the label will remember the font and render in that font on the device.

For Linux/other: use the Java editor, and make sure Java works when you double click the editor .jar file.
Since you are more savvy, It's up to you to install/configure pd-vanilla properly:
http://puredata.info/downloads/pure-data
If you want to have editor-to-pd communication (to simulate app behavior), then you need to add the following externals (all by Martin Peach) to the pd object path: udpsend, udpreceive, packOSC, unpackOSC, routeOSC.
https://pure-data.svn.sourceforge.net/svnroot/pure-data/trunk/externals/mrpeach/osc/
https://pure-data.svn.sourceforge.net/svnroot/pure-data/trunk/externals/mrpeach/net/

====Learn the workflow by making your first piece - Hello, sine wave world!
1. Open MobMuPlatEditor
-Under the "Doc" tab, select your device under "Screen Size"
-Under the "Add" tab, click "slider", and drag/resize it on the canvas until it is nice and big
-Under the "Prop" tab, note the OSC-style address ("/mySlider"). Each GUI object should have a unique name. Leave it for now, though.
-The default range of a slider is 2, which means it sends continuous values 0-1. Change the range to 12, and it will send out integer values 0-11
-Hit the "Lock" tab, the editor is now in locked mode, so drag the slider and see the outgoing messages in the console
e)Save it, give it a distinctive name. it will create a ".mmp" file. (which is a JSON-style string, in case you care about that). Keep the editor and this file open.

2. Open Pd
-open the patch “PdWrapper.pd". Don't change anything in this patch, this patch allows communication between the editor and your Pd patch. 
-open the patch "MMPTutorial0.pd". Take a look at it. When you slide your slider in the editor, its value should show up in the patch. In the Pd window (apple-r) click "DSP" to turn on audio. You should hear a tone that responds to the slider. Feel free to tinker around in this patch before continuing.
-go back to the "doc" tab of the Editor, hit "Choose", and select the "MMPTutorial0.pd" patch. 

3. Connect your iOS device to your computer and open iTunes
-click the device in the left bar of iTunes, and then click "apps" along the top bar
-scroll down to the section "Apps with Documents", and then click MobMuPlat. If you've already used MobMuPlat on the device, then you'll see a list of files (.mmp files, .pd files, sound files, graphics files). Drag your saved .mmp file AND the "MMPTutorial0.pd" into this box, and they will be automatically copied onto the device.

4. On your iOS device, open MobMuPlat
-click the round info button on the welcome screen	
-on the load screen, find your distinctively-named file, and click it
-you should see your slider. drag it up and you should hear your pitch. Congrats, you have made something! 

5. ???

6. PROFIT!

===Get more into it
Check out the series of tutorials (installed in the app, and also included in the software package), to see how various features work: GUI elements, networking, sampling, image files, hardware info, and more!


===Contact
questions? bug reports? email: info -AT- mobmuplat.com 
