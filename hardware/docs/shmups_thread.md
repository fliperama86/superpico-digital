# Sharp analog RGB for the 3-Chip SNES using digital signals

---

## #1 — Opatus
*Tue May 26, 2020 11:55 am*


**EDIT

The PCB files (mod board and 2 adapters) have been made public (source, bom and gerbers): https://github.com/Opatusos/SNES_TST

Please note that the newest version of the boards has not been tested. Also the included firmware is only a test firmware and it might not even be compatible with the new version of the mod board.

It seems like that all of the previously discussed problems are solved which results in a very clean RGB output.

The board includes the following mods as well: dejitter, CIC, D4 patch, IGR, dual oscillator. Not all functions are tested and/or implemented. The board also has the option to clean the composite video. 

/EDIT**

I have recently followed up the work started on the Digital Video output of the SNES using the TST pins. (2-Chip  SNES, PPU2).

I've found the topic on the German forums: [https://circuit-board.de/forum/index.ph ... IP-APU-DSP](https://circuit-board.de/forum/index.php/Thread/25396-SNES-Chips-decapped-2PPU-1CHIP-APU-DSP)

It was found that if the TST15 pin of PPU2 is pulled high then the TST0-TST14 pins are corresponding to the RGB video. (TST0-TST4: Red; TST5-TST9: Green; TST10-TST14: Blue). 

There were some issues though. The brightness control was not included in the digital output and at a certain Mode 7 setting the picture was missing outside the active Mode 7 area.

As I don't like how blurry the 2-Chip RGB signal looks (even with reverse LPF on OSSC), I wanted to give it a go. The first time I've connected the green TST pins as a proof of concept and it looked promising. I have also found that when I pull the OVER1 and OVER2 PPU2 pins high than the Mode7 works as intented.

Then I've made 2 PCBs and removed the PPU2 from the board and mounted it on my board which I have connected to the SNES through an adapter PCB. I've tried it on my broken SNES first (does not show the sprite layer). After some tests and slight CPLD code adjustments I have tested it in my working SNES unit as well. The brightness setting works and in Pilotwings the start screen looks like it should (uses the problematic Mode 7 settings).

There are still some problems though. Every 10-20 seconds there is (probably) 1 frame of flicker in most games (only 1 layer, mostly (if not only) the sprite layer (I did not have that issue with the broken SNES). There is a possible workaround by turning the digital output off during sync but I have not checked it yet. 

There is some noise on my DA converted RGB signal, it can be improved upon.

At the start of Super Mario World when the logo shows up the background is only garbage, I did not look into that issue.

The original analog video is connected to the original encoder for composite output but the colors are way off, not sure why. The original RGB signal is fine only the composite has the problem.

On certain colors slight diagonal lines can be seen (was there before modification as well), most likely cable issue.

Overall everything works. I have tested all my games (ca. 20 PAL games) I have not seen anything strange other than the issues discussed above.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/9wiTPiK.jpg)

Here is a video of Yoshi's Island with 1 frame of flicker at around 0:03: 

https://youtu.be/cSWN-uA_Ibc

And here are photos and videos of the project (unforunately I couldn't do a direct capture):

[https://www.dropbox.com/sh/5pgdqq6ppjmb ... ffgza?dl=0](https://www.dropbox.com/sh/5pgdqq6ppjmb4co/AAC5k4k9Di1v9dcw7J4bffgza?dl=0)

All photos are taken using the OSSC in Line5X mode on a 4k Samsung TV.

---

## #2 — yoshiyukiblade
*Sat May 30, 2020 9:10 am*


This looks very promising! If all the remaining issues can be worked out, then we can finally have a perfect video solution. Best part is that it's the original non-1CHIP hardware.

I've been working on a pure analog solution to clean up the picture, but it will never look that good. Maybe I can finally put this project to rest.

---

## #3 — captaineos
*Sun May 31, 2020 2:12 am*


I read through your posts on the DE forum and then found them here.  So great to bring awareness to your efforts as they appear to be a great idea and potential solution for us all.  

I have put a fair amount of effort into my 2-chip PPU PAL SNES but still get sad when I see the blurring of black lines.  Some games are worse than others.  E.g I don't really notice it on Super Mario Kart or Super Street Fighter but 240p text suit and Zelda are definitely too soft. 

My 80s computing understanding of digital RGB is that 8 colours can be pushed to 16 with intensity but that's the cap for TTL RGB.  When you say digital RGB but show a beautiful gamut on your TV, does this mean it's not TTL RGB but something else?  The closeups of SMW and Zelda are so crisp and colourful that I am amazed!

By all means keep us updated as I'm keen to keep my existing hardware and upgrade the RGB out with your ideas.

---

## #4 — Unseen
*Sun May 31, 2020 10:26 am*


> 

captaineos wrote:When you say digital RGB but show a beautiful gamut on your TV, does this mean it's not TTL RGB but something else?

CGA (what you call TTL) is also digital RGB, but it uses just one bit per color channel and one additional bit for global brightness, so you can have two different values per color channel multiplied by two possible global brighenesses, giving 2*2*2*2=16 colors in total.

The digital RGB data on the SNES' test port is 5 bits per color, which results in 2*2*2*2*2=32 different shades for each color channel or 32*32*32=32768 colors total. There is an additional 4 bit wide brightness register in the PPU that can globally make the screen darker (e.g. for dafing the screen), but calculating how many additional colors can be created using it is a bit more complicated.

---

## #5 — Harrumph
*Mon Jun 01, 2020 1:18 pm*


This is great, seems to have overcome some hurdles that were not thought possible before!

> 

Opatus wrote: 

After some tests and slight CPLD code adjustments I have tested it in my working SNES unit as well. The brightness setting works and in Pilotwings the start screen looks like it should (uses the problematic Mode 7 settings).

So how did you actually solve the part with the brightness setting, you can tap if from somewhere else?

I checked out the original thread briefly, good to see the major players on the german scene all seems to have been involved (Ikari, Unseen, Borti etc). ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/icon_smile.gif)

---

## #6 — Opatus
*Tue Jun 02, 2020 9:18 am*


I did some more tests. First I have connected Csync to TST15 to solve the issues with the flicker. With that the flicker got slightly better.

After that the TOUMEI signal (it is high when there is a valid picture and low during sync and burst) was connected. This did solve the SMW start screen problem. During the test of Super Castlevania IV and Yoshi's Island everything seemed right but F-Zero had the issue. It might be related to heat or soldering.

The composite color problem seems to be the missing blue color. The PPU2 might be damaged (it is missing at least 10 pins, I had to replace them with thin wires). I will desolder it and use the PPU2 from my working SNES.

@yoshiyukiblade

Was it the japanese solution or something different?

@captaineos

These were not my ideas only the OVER1 and OVER2 signals, which for the time being seems to solve the problem with Mode7. The colors are virtually identical to the original but the image is much sharper.

@Harrumph

The brightness was never a real issue as the value can be easily read when the PPU register $2100 is set.

---

## #7 — yoshiyukiblade
*Tue Jun 02, 2020 9:00 pm*


> 

Opatus wrote:
Was it the japanese solution or something different?

It's my own design, but largely inspired by the Japanese mod. I think I'll keep working on it until I'm mostly satisfied with the result. It's still "safe" in the sense that it's based on the PPU's analog output and nothing else, but of course it's not the best quality attainable. Anyway, I look forward to see where the digital mod goes!

---

## #8 — Opatus
*Fri Jun 05, 2020 10:14 pm*


I think I've found your channel on Youtube. The last video looked really nice with the mod.

I have changed the PPU2 and the flicker is gone. So it might have been just a soldering issue. 

The composite output looks the way it should as well, the blue color was missing before.

I had to revert back to csync for the digital output enable signal. The start screen of Super Mario World is broken again. It seems that it can be solved with a quick automatic reset after the system has powered up.

Beside a slight noise on some colors the output looks perfect with an OSSC. But it could be problematic with a CRT as there is a 50-100ns ringing at color changes. It comes from the way the brightness is mixed with the RGB signal. I create a voltage reference with a buffered R2R DAC and the digital signals coming from the PPU are switching between this brightness reference voltage and GND. These are then ran through the color R2R DACs. The problem is that at every digital signal edge the voltage reference gets pulled and starts ringing. 

If someone has an idea how the brightness could be combined with the RGB signal I am open for it.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/LfvIRHz.jpg)

---

## #9 — rama
*Sun Jun 07, 2020 2:05 pm*


Awesome that there's a new development here. Congrats! ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/icon_smile.gif)

The brightness problem might be solvable with a digital processor / scaler board.

I could attempt something with the digital inputs on gbscontrol. Just need to find time to do it.

---

## #10 — Opatus
*Wed Jun 10, 2020 2:00 pm*


Yeah I think I will go the digital route.The reference voltage has to change too quickly and thus it is not strong enough for the DAC. I've added a capacitor (1uF) which made the noise mostly disappear but it has also killed the rise/fall time of the reference voltage. As I have read even Nintendo had the same problem with the 1Chip SNES.

The rise time issue on top of the image in Yoshi's Island:

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/jTdqHNe.jpg)

So I wrote a quick test code for the smallest MAXII CPLD and it seems that 3 5X4 bit multipliers will fit (or worst case a LUT for the multiplication results) and the 3x9 bit results can go to a ADV7123 for conversion.

The current version already has the dejitter function and provision for the $2134-D4 region patch (so the output pin of the PPU does not have to be overdriven). I will also add the SuperCIC and IGR as most of the data for those are already accessed. And everything should fit under the top shielding.

---

## #11 — Voultar
*Thu Jun 11, 2020 1:35 am*


> 

Opatus wrote:Yeah I think I will go the digital route.The reference voltage has to change too quickly and thus it is not strong enough for the DAC. I've added a capacitor (1uF) which made the noise mostly disappear but it has also killed the rise/fall time of the reference voltage. As I have read even Nintendo had the same problem with the 1Chip SNES.

You are correct. In the 1964 I determined that the current-steering DAC in the 1CHIP ASIC was not able to stabilize the swing for the video quickly enough. Slightly increasing the capacitance helps with the ghosting/visual noise, but at the expense of increasing the ready-state of the video lines, resulting in that slightly dark line that you see. 

This all looks fantastic, BTW. If there's anything that I may do to help you. Let me know!

---

## #12 — yoshiyukiblade
*Fri Jun 12, 2020 11:04 pm*


I'm thinking about abandoning my previous RLPF project now that this breakthrough is progressing very well. I did a long 7-hour test a few days ago with Zelda 3, but it's still not quite there yet: https://www.twitch.tv/videos/645520068. The digital mod is clearly the way of the future, but it seems pretty complicated with regard to getting the brightness values. I might start small with learning how to make a quality DAC from the digital RGB signals first, then move forward from there. I'm still a novice in electric circuits, especially relating to digital stuff, so this is all still very new and exciting to me!

---

## #13 — rama
*Sat Jun 13, 2020 12:07 am*


yoshiyukiblade:

Your work on restoring the analog output is really good!

Please don't let it go to waste ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/icon_smile.gif)

It is relevant to all kinds of analog signal issues, and it could help improve some other console!

---

## #14 — Unknown
*Sat Jun 13, 2020 3:38 am*


> 

Voultar wrote:

> 

Opatus wrote:Yeah I think I will go the digital route.The reference voltage has to change too quickly and thus it is not strong enough for the DAC. I've added a capacitor (1uF) which made the noise mostly disappear but it has also killed the rise/fall time of the reference voltage. As I have read even Nintendo had the same problem with the 1Chip SNES.

You are correct. In the 1964 I determined that the current-steering DAC in the 1CHIP ASIC was not able to stabilize the swing for the video quickly enough. Slightly increasing the capacitance helps with the ghosting/visual noise, but at the expense of increasing the ready-state of the video lines, resulting in that slightly dark line that you see. 

This all looks fantastic, BTW. If there's anything that I may do to help you. Let me know!

What does that cap do? Is it decoupling for some internal reference? Just throwing out a wild idea, any merit in building a circuit that actively buffers the voltage?

---

## #15 — Unknown
*Sat Jun 13, 2020 3:41 am*


This is some truly amazing stuff.

You probably already thought of this, but I might suggest using a Video DAC instead of a R2R ladder: [https://www.analog.com/media/en/technic ... DV7125.pdf](https://www.analog.com/media/en/technical-documentation/data-sheets/ADV7125.pdf)

This will probably get rid of all your artifacts and generate perfect video levels.

---

## #16 — yoshiyukiblade
*Sun Jun 14, 2020 12:18 am*


@rama

Okay, maybe I'll just put it on the shelf instead of abandon it. ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/icon_smile.gif) I'll revisit it whenever it becomes relevant again. Optimal sampling is a smoke screen for the underlying issues that are still there. I wanna see how it can look like with a perfect source!

@mikechi2

That may save a lot of headaches about DACs. Forgive my newness to this, but can it scale down from 8 bits to 5?

I started learning about weighted sum DACs and did some basic LTSpice simulations. One problem is that the glitches can be quite large. You can see similar glitches quite clearly on the 2chip internal DAC (8x oversampling on OSSC): https://i.imgur.com/cVsCPf4.png. The 1CHIP's DAC remains quite clean: https://i.imgur.com/LpKwrtY.png. I haven't taken a look at it on the scope, but any glitches are so small that you don't really see it even with 8x oversampling. Soon I'm gonna get my backup stock SNES motherboard, lift the relevant pins, then grab some real data to simulate with.

---

## #17 — Unseen
*Sun Jun 14, 2020 12:34 am*


> 

yoshiyukiblade wrote:That may save a lot of headaches about DACs. Forgive my newness to this, but can it scale down from 8 bits to 5?

Connect the 5 SNES bits to the high-order bits of the DAC and either leave the three lowest bits grounded or connect the three highest-order bits of the SNES to the lowest-order bits of the DAC. The first method reduces the maximum brightness slightly which could be compensated by tweaking Rset, the second method reaches the full brightness, but introduces a slight nonlinearity.

For the SNES this isn't a problem though, after multiplying the 5 bit color value with the 4 bit content of the brightness register each color channel is 9 bits wide and that extra bit must be either dropped or processed into the other 8 to feed that DAC.

---

## #18 — yoshiyukiblade
*Sun Jun 14, 2020 2:40 pm*


So there are a number of ways to approach this. That's pretty neat.

I did some of the modifications posted in this thread and started poking around a bit. Lifted pins 37, 50 (tied both to VCC), and lifted 90-93. Pin 93 (TST15) was tied to the buffered CSYNC line. It seems to draw a fair bit of current because I originally tied it to pin 100, but the unbuffered CSYNC signal distorted pretty badly.

The analog video output looks mostly good, but I did notice some mode7-related glitches. From what I've read, glitches occur in both the analog and digital outputs. I'm not sure if this was a problem that was eventually solved by other tweaks.

Chrono Trigger pendulum: https://i.imgur.com/kZox8Y4.png When the pendulum swings far right, this graphic swings into view briefly

Super Metroid Ceres: https://i.imgur.com/eb716R2.png Multiple copies of Ridley fly out

Super Metroid planet Zebes arrival: https://i.imgur.com/OMOnSj2.png Multiple copes of the ship appear as it rotates counter clockwise

Anyway, I did start grabbing scope data and used it in my simulated DAC circuit with success.

---

## #19 — Opatus
*Sun Jun 14, 2020 10:28 pm*


> 

This all looks fantastic, BTW. If there's anything that I may do to help you. Let me know!

Thank you but right now I don't think you can help me as I will have to redesign my board.

> 

What does that cap do? Is it decoupling for some internal reference? Just throwing out a wild idea, any merit in building a circuit that actively buffers the voltage?

I believe it is the buffer capacitor for the reference voltage and it is already active. I'm just guessing though as I don't have a 1Chip Snes. I had one but I sold it as I did not know it had a nicer picture.

> 

You probably already thought of this, but I might suggest using a Video DAC instead of a R2R ladder: https://www.analog.com/media/en/technic ... DV7125.pdf

I wrote above that I will retry it with a ADV7123, which is a 10bit DAC. I don't want to drop bits to make it as accurate as possible.

> 

I'm thinking about abandoning my previous RLPF project now that this breakthrough is progressing very well. 

Please don't, more options are always welcome and you also learn a lot doing this.

> 

The analog video output looks mostly good, but I did notice some mode7-related glitches. From what I've read, glitches occur in both the analog and digital outputs. I'm not sure if this was a problem that was eventually solved by other tweaks.

That is bad. I don't have these games unfortunately. I only have Pilotwings with this Mode7 option. Do you have that? Could you verify it?

I had no problem with using Pin100 on TST15, or at least I did not notice any. 

Could you try connecting only one OVER pin to Vcc separately. The other should go to the original pad. Maybe it changes the problem, but it will probably not solve it.

As a last resort the digital output could be turned off and the output reverted back to the original RGB video for this special Mode7. It should be possible to do it seamlessly and as this mode was not really used in games it should have almost zero effect on the overall performance.

Do you have pulldown resistors for TST12-TST14? If they are floating it can lead to false colors.

---

## #20 — yoshiyukiblade
*Mon Jun 15, 2020 12:06 am*


> 

Opatus wrote:
That is bad. I don't have these games unfortunately. I only have Pilotwings with this Mode7 option. Do you have that? Could you verify it?

I had no problem with using Pin100 on TST15, or at least I did not notice any. 

Could you try connecting only one OVER pin to Vcc separately. The other should go to the original pad. Maybe it changes the problem, but it will probably not solve it.

As a last resort the digital output could be turned off and the output reverted back to the original RGB video for this special Mode7. It should be possible to do it seamlessly and as this mode was not really used in games it should have almost zero effect on the overall performance.

Do you have pulldown resistors for TST12-TST14? If they are floating it can lead to false colors.

I forgot to mention that I used an SD2SNES to test these games. I don't know if the behavior changes depending whether it's the original cart or not. The Pilotwings USA opening looks fine, but the gameplay portion is a completely black screen. All the remaining TST pins (0-14) are floating, but I haven't noticed any color issues. I'll bodge pulldown resistors to see if it does anything different. Everything looks pretty normal in the various games I loaded up, except for the mode 7 glitches I spotted and the black screen I just discovered on Pilotwings. edit: black screen occurs on my main console. This was a SD2SNES issue because I forgot that I erased the dsp roms during a reformat. False alarm!

Pin 100 to TST15 still functions properly on the OSSC; it was still able to maintain sync and I got a stable picture, but the signal fell to less than 2 Vpp and looked distorted on the scope. My guess is that it draws enough current to cause this distortion.

I am on PPU revision C, if that matters.

---

## #21 — yoshiyukiblade
*Mon Jun 15, 2020 12:48 pm*


I tested some combinations of OVER1/2 with VCC and the original OVER bus, but found nothing particularly useful. Leaving OVER2 floating seemed to function the same as pulling both high. Putting OVER2 back on the original OVER bus results in some additional minor glitches in the problem screens (in CT and Pilotwings). Other combinations resulted in glitches ranging from minor to severe.

As a sanity check. I put everything back to mostly stock form and the glitches went away. TST15 has to be pulled low or the glitches remain if left floating. I left TST12-14 floating, but that didn't seem to affect anything.

Out of curiosity, I tied TST15 to VCC and it seems to function the same as using the CSYNC signal (with OVER1/2 pulled high as well). I read some old discussion about using CSYNC to fix some issues that I haven't seen when OVER1/2 are pulled high. All of this this analysis is done on the analog output though. I'm not sure if there are significant differences in the digital output.

Do you think checking the brightness register can result in all the different problems you've seen?

---

## #22 — Unseen
*Mon Jun 15, 2020 3:19 pm*


> 

yoshiyukiblade wrote:Out of curiosity, I tied TST15 to VCC and it seems to function the same as using the CSYNC signal (with OVER1/2 pulled high as well). I read some old discussion about using CSYNC to fix some issues that I haven't seen when OVER1/2 are pulled high.

IIRC with TST15 pulled high permanently, Pilotwings has a broken palette and the pendulum in the intro of Chrono Trigger is missing while it is still in its swinging animation phase. I think this applied both to the digital TST output as well as the analog one.

---

## #23 — Opatus
*Tue Jun 16, 2020 10:49 am*


> 

IIRC with TST15 pulled high permanently, Pilotwings has a broken palette and the pendulum in the intro of Chrono Trigger is missing while it is still in its swinging animation phase. I think this applied both to the digital TST output as well as the analog one.

For me Pilotwings worked fine if the pins TST12-TST14 were pulled down and TST tied to Vcc. Without pulldown resistors however it was unstable.

> 

I forgot to mention that I used an SD2SNES to test these games.

I don't think it should matter but who knows, I don't have one to test it with.

It is strange that your TST15 pin draws a lot of current, it is an input-only pin and therefore it should be high impedance.

The brightness register is unrelated. The analog output is generated inside the PPU from the digital signals that are also on the TST pins, so they should be identical.

I have combined the HBLANK, VBLANK and PED signals to turn off the digital output when these are active. I have also ordered a japanese copy of Chrono Trigger but it will take weeks to arrive. Then I can experiment a little with the pendulum. I am thinking about picking up a Super Metroid copy as well.

Could you upload a video of the Super Metroid problems? They look very strange, the copies are not identical (maybe DMA/IRQ problem?)

I have tested the Super Castlevania IV Level 4-4 boss. It is a Mode7 boss but probably not the problematic Mode7, but it looked good. The tunnel (Level 4-3) looked good as well, but it is also not the problematic one.

---

## #24 — yoshiyukiblade
*Tue Jun 16, 2020 3:25 pm*


I don't know for certain how much current TST15 is drawing, but I'm not sure what else can cause the unbuffered CSYNC signal to distort.

Here are a couple clips of the Super Metroid problem:

https://youtu.be/8UojiuEssK8

https://youtu.be/ul37xbi9kS8

Other than these few issues I noticed, it has been working remarkably well. I haven't gotten any weird issues with color, sprite flicker, etc. I just have TST15, OVER1, OVER2 tied to VCC, and TST12-14 left floating.  I wonder if NTSC vs PAL have differences.

I'm slowly deciding on the parts to use for a DAC prototype. Since the digital pulses are used directly in the construction of the analog signal, I want to run all signals through high speed comparators to generate clean rectangles. I'm not sure if that's what you did on your board, but I think it's a good idea.

---

## #25 — Opatus
*Tue Jun 16, 2020 3:50 pm*


Thank you.

It seems like the Mode 7 is simply in screen repetition mode, strange. Not sure why do they look different, it is probably just because of the low resolution scaling.

I have just bought a copy at Ebay. Hopefully I recevie it until Friday.

If you touch the TST12-14 signals with your fingers you might get the color issues but maybe only when you power the system on, as TST15 is tied to VCC.

The rise time for the signals are decent so I use them directly with an analog switch to switch between the brightness voltage and GND. I don't think sharper rise times matter, certainly not for the OSSC and most likely not even for a BVM.

---

## #26 — Unseen
*Tue Jun 16, 2020 8:31 pm*


> 

yoshiyukiblade wrote:Since the digital pulses are used directly in the construction of the analog signal, I want to run all signals through high speed comparators to generate clean rectangles. I'm not sure if that's what you did on your board, but I think it's a good idea.

I think it's pointless to do that. If you use an integrated DAC, it will have an internal latch driven by the pixel clock that captures the digital value of the signals; if you want to build a discrete DAC, just add pixel-clock-driven latches yourself, e.g. using two 74HC573.

> 

I just have TST15, OVER1, OVER2 tied to VCC

I'm surprised there aren't any graphics issues with OVER1/2 tied high, although I can't point at any specific title where I would expect any.

---

## #27 — yoshiyukiblade
*Wed Jun 17, 2020 1:14 am*


@Unseen

I want to build a discrete one mostly as an academic exercise. I didn't know that latches existed lol. I'll look more into this direction and make simulations where possible. Thank you for the suggestion! 

In the design I'm going for, the pulses need to be stable under some amount of resistive load for the summing amplifier. I don't want to use huge resistors, so devices that can source and sink a fair amount of current is ideal. The summing amp is non-inverting, so all 5 pulses need low impedance outputs for the weights to be accurate with respect to the resistor values.

@Opatus

Though rise times are not an issue, I'm concerned about noise patterns that are coupled in with the pulses. The OSSC might not show it with optimal sampling, but it might be visible at other sampling rates. For instance, the repetitive vertical bars at about 21 MHz are definitely visible at very high sample rates (2728 samples per scanline) even on a 1CHIP. Other patterns can be diagonals, scrolling or fixed. Probably not visible on a CRT for sure. Maybe I'm just paranoid for looking at this too long ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/icon_smile.gif). I'm wondering if I can get rid of all that just by generating new pulses with a regulated low noise voltage source. It's a little over the top, but all in the name of curiosity!

---

## #28 — Opatus
*Wed Jul 01, 2020 12:25 pm*


I did not have much time to post here in the last 2 weeks.

In the meantime I've received Super Metroid and I had the same issues as well. I've also found a cheap PVM not far so I could check the mod on a crt. It looks great even with the broken DAC. Here's an image of it:

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/A4lj9Jq.jpg)

I've also tested Air Strike Patrol (Desert Fighter to be exact) to see if it is handled well. And as it was expected worked really well. The Hi-res mode showed no problem and the jetplane shadow (the mid-scanline brightness change) was also there. It was a bit faint due to the capactior on the reference voltage but it was visible.

For the mod itself as of right now I see 3 possbile solutions:

1. The already mentioned switching between the original and digital RGB, just like the 32X does. Of course it would not really be suited for an HMDI mod, also the quality difference can be a bit jarring. And third the difference in voltage for the 2 RGB signals. That means that after the digital RGB comes out of the DAC it has to be clamped otherwise there will be a difference in black levels. It is of course not a big problem.

I have already tried this looking at the composite output. It works, although I have seen a ship duplicate in Super Metroid during the intro. I think it happens because the PPU pipeline is a few pixels deep and I use OVER from PPU1 to switch between the signals. So this OVER has to be delayed and the duplicate should in theory disappear.

2. Change the PPU to the mode where it outputs character #0 when the screen wraps around. The only problem is that it is not guaranteed that character #0 is transparent. Most likely it is but as the Chrono Trigger image from yoshiyukiblade shows that the VRAM is already preloaded with data (not character #0 though). Most likely no game has any color at the first character but I don't know it.

3. If I'm correct this could be the real solution. I don't know how exactly the Mode7 is communicated between the 2 PPUs I'm just guessing, so please correct me or confirm it. I think the PPU1 calculates the VRAM coordinates for every pixel and then reads it from VRAM. PPU2 also receives this pixel as it listens to the bus and that's how it knows what color to use. So if that is correct all we have to do is when OVER from PPU1 is low set the VRAM data that the PPU2 receives to 0 and pull the OVER for the PPU2 high. That way the PPU2 will think the screen has not wrapped around yet  and will output color 0 (which it should normally do anyway). If this proves correct then this would be an en easy way the correct the problem.

I've also seen an issue during the Donkey Kong Country level "Millstone Mayhem". Half of the screen went green for a frame every 3-4 seconds (always at the same section of the level, probably Rare is doing something unorthodox once again). I was switching TST15 off during HBLANK, VBLANK and forced blanking. After that I've tied TST15 to Vcc -> the problem went away but the color palette got broken again. I will look into the exact timing and how to avoid it later.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - shmups.system11.org_files/RT37z9O.jpg)

---

## #29 — Unseen
*Wed Jul 01, 2020 3:05 pm*


> 

Opatus wrote:I think the PPU1 calculates the VRAM coordinates for every pixel and then reads it from VRAM. PPU2 also receives this pixel as it listens to the bus and that's how it knows what color to use.

My current understanding is that PPU1 gathers all potential data sources for the current pixel and sends them to PPU2, which applies palettes, colormath and at least some priorities(*) to calculate what the final pixel should look like.

(*) can't remember off-hand if PPU1 sorts out the priority between tile layers itself or not

> 

That way the PPU2 will think the screen has not wrapped around yet  and will output color 0 (which it should normally do anyway). If this proves correct then this would be an en easy way the correct the problem.

I don't think this is always correct, the game can select between three different wrapping methods (wrap within 128x128 tile map, use transparency, use tile 0). According to my old notes, /OVER from PPU1 is the signal that indicates if the current pixel is in the wrapped part of the mode7 area. IIRC the title screen of On The Ball uses wrapmode 2 (transparent).

---

## #30 — Opatus
*Wed Jul 01, 2020 3:21 pm*


> 

I don't think this is always correct, the game can select between three different wrapping methods (wrap within 128x128 tile map, use transparency, use tile 0). According to my old notes, /OVER from PPU1 is the signal that indicates if the current pixel is in the wrapped part of the mode7 area. IIRC the title screen of On The Ball uses wrapmode 2 (transparent).

Sorry I've meant only for this wrap mode. So do that if transparency should be used otherwise use the data given by PPU1.

I thought those pins are used for the sprites. They would not be enough for 256 or direct color backgrounds and 2 bits of priority. The PPU2 has to do the window effects so I assume it should know everything about the sprites and the backgrounds. It also does mosaic (according to the manual) and it is only performed on the backgrounds.

I will poke around a little bit if I find the time.

---

## #31 — Unseen
*Wed Jul 01, 2020 6:39 pm*


> 

Opatus wrote:The PPU2 has to do the window effects so I assume it should know everything about the sprites and the backgrounds. It also does mosaic (according to the manual) and it is only performed on the backgrounds.

Maybe mosaic is split between the PPUs? PPU2 is not connected to the video address bus, so PPU1 always determines what data is fetched, but PPU2 could decide to throw some of it away and just repeat the previous value to create the horizontal aspect of the mosaic effect.

---

## #32 — Opatus
*Thu Jul 02, 2020 12:23 pm*


Yes, you are right, then it must be split betweeen the two and the manual is incorrect or half correct.

---

## #33 — Opatus
*Fri Jul 03, 2020 8:51 am*


After shorting a few pins with a pair of tweezers (very professional) I can confirm that the CHAR, COLOR and PRIO pins are used for the sprites, at least in Mode 7. And as expected the graphics data comes from the U4 SRAM chip.

So if the timing of the OVER signal is correct, this solution should work.

---

## #34 — yoshiyukiblade
*Sat Jul 04, 2020 4:32 pm*


The PVM screenshot looks good! I'm sure any improvement from here is negligible for analog applications. I have a board on the way for a basic DAC. No circuitry for brightness control yet. I'm not smart enough to investigate anything beyond this point, but I'll post results of my DAC when it comes (assuming it works!).

---

## #35 — Opatus
*Sun Jul 05, 2020 11:03 pm*


I've made a little test to prove the concept by lifting the LSB of the data line and connecting it to the CPLD. In Mode 7 transparent mode the line is pulled high when the OVER signal is low, otherwise the original data is passed through. On the image below it can be seen that it works, the timing of the OVER signal is perfect for our purpose (which is not a big surprise) and the sprites remained intact.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 2 - shmups.system11.org_files/vdUSWqz.jpg)

I've also changed the timing of the digital output deactivation: turned off during vsync and forced blanking. This seems to have solved the issues with Donkey Kong without the palette color problems.

Next step is a new board with all the improvements.

@yoshiyukiblade

Please keep us updated with your progress.

---

## #36 — Opatus
*Mon Jul 13, 2020 8:34 pm*


I add an image for clarity, (this was taken after I changed the fuse in my SNES and repaired my improvised programmer). Now the data bit0 for the PPU2 is negated and every pixel which should be transparent has its color changed. The sprites and the non-transparent background remain unchanged.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 2 - shmups.system11.org_files/HPemy77.jpg)

---

## #37 — waterbottle
*Sat Aug 15, 2020 9:05 pm*


@Opatus

This is incredible work. I know that byuu was calling for work like this as a way to figure out accurate cycle times for the PPU [https://arstechnica.com/gaming/2020/04/ ... erfection/](https://arstechnica.com/gaming/2020/04/how-snes-emulators-got-a-few-pixels-from-complete-perfection/) Last I heard they contracted someone to do 100x die scans of the PPUs but your mod may accelerate their work, thought I am not sure if there is a specific torch bearer on that project anymore since byuu has retired.

Is there a possibility for an HDMI mod? I know that the NTSC/HDMI timings aren't compatible but maybe you could make something that has 1-frame buffer delay? Not sure.

I cannot tell for certain but it seems that it's possible to get a pixel perfect picture once all the relevant signals are analyzed and accounted for. What do you think? Do you need an SD2SNES cart? A spare SNES/SFC/PAL SNES? I can possibly help provide you with those tools.

---

## #38 — Opatus
*Tue Aug 18, 2020 10:02 pm*


I've read that article a few months ago and then I was surprised nobody looked into it further if it was that well known. I did not know that byuu retired, I've read his farewell now, it's really sad.

The HDMI mod will happen eventually just not by me, provided everything works as intended.

I did not know why would have byuu need a perfectly working digital mod to analyze the signals. The parts that were not working could have been ignored as these had basically nothing of value (almost) for emulation.

Thank you for your offerings but I think I don't need them. The PPU chips are identical for every regions and I have 20-30 carts, it should be sufficient to decide if everything is working or not. Also I'm not analyzing the signals in depth, I am just using them for the video modification and optimizing the timing if needed.

---

## #39 — Opatus
*Wed Sep 30, 2020 7:40 pm*


Just a quick update. The new board is ready and new results can be expected in 2-3 weeks.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 2 - shmups.system11.org_files/Z1NxaBH.png)

---

## #40 — yoshiyukiblade
*Fri Oct 02, 2020 12:10 am*


Looks exciting! Hoping for positive results.

---

## #41 — Lawfer
*Fri Oct 02, 2020 10:05 pm*


What's the difference between the 3-Chip and 2-Chip? I have heard people saying they don't want to get anywhere near the 1-Chip version because it's "not the original" or whatever, but then seems like there isn't two types of SNES consoles, but 3 types "3-Chip", "2-Chip" and "1-Chip", so then that means 2 two of these are "not the original" and only one is, so which is the original SNES design, 3-Chip or 2-Chip?

---

## #42 — Unseen
*Sat Oct 03, 2020 12:07 am*


> 

Lawfer wrote:What's the difference between the 3-Chip and 2-Chip?

Different ways of counting the chips on the same board. 3-Chip counts all "major" chips (CPU, PPU1, PPU2), 2-Chip only counts the PPUs.

> 

so which is the original SNES design, 3-Chip or 2-Chip?

Both and neither. =) The original SNES design had the sound subsystem on a separate module and the CPU+PPU1+PPU2 on the main PCB. The sound stuff was integrated into the main PCB in later revisions which still had the same CPU+PPU1+PPU chips, so these boards are still called 2/3-chip SNESs. The actual change that does make a difference is the one to the 1-Chip version which combines the CPU and both PPUs into a single chip, but the sound subsystem is still separate.

---

## #43 — Lawfer
*Sat Oct 03, 2020 1:46 pm*


> 

Unseen wrote:Different ways of counting the chips on the same board. 3-Chip counts all "major" chips (CPU, PPU1, PPU2), 2-Chip only counts the PPUs.

Both and neither. =) The original SNES design had the sound subsystem on a separate module and the CPU+PPU1+PPU2 on the main PCB. The sound stuff was integrated into the main PCB in later revisions which still had the same CPU+PPU1+PPU chips, so these boards are still called 2/3-chip SNESs. The actual change that does make a difference is the one to the 1-Chip version which combines the CPU and both PPUs into a single chip, but the sound subsystem is still separate.

So wait, if I understand correctly you are saying that 3-Chip and 2-Chip SNES are just different names for the same exact model? Is that it?

---

## #44 — maxtherabbit
*Sat Oct 03, 2020 2:10 pm*


> 

Lawfer wrote:

> 

Unseen wrote:Different ways of counting the chips on the same board. 3-Chip counts all "major" chips (CPU, PPU1, PPU2), 2-Chip only counts the PPUs.

Both and neither. =) The original SNES design had the sound subsystem on a separate module and the CPU+PPU1+PPU2 on the main PCB. The sound stuff was integrated into the main PCB in later revisions which still had the same CPU+PPU1+PPU chips, so these boards are still called 2/3-chip SNESs. The actual change that does make a difference is the one to the 1-Chip version which combines the CPU and both PPUs into a single chip, but the sound subsystem is still separate.

So wait, if I understand correctly you are saying that 3-Chip and 2-Chip SNES are just different names for the same exact model? Is that it?

3 chip and 2 chip refer to the same group of models

---

## #45 — Lawfer
*Sat Oct 03, 2020 2:35 pm*


> 

maxtherabbit wrote:

> 

Lawfer wrote:

> 

Unseen wrote:Different ways of counting the chips on the same board. 3-Chip counts all "major" chips (CPU, PPU1, PPU2), 2-Chip only counts the PPUs.

Both and neither. =) The original SNES design had the sound subsystem on a separate module and the CPU+PPU1+PPU2 on the main PCB. The sound stuff was integrated into the main PCB in later revisions which still had the same CPU+PPU1+PPU chips, so these boards are still called 2/3-chip SNESs. The actual change that does make a difference is the one to the 1-Chip version which combines the CPU and both PPUs into a single chip, but the sound subsystem is still separate.

So wait, if I understand correctly you are saying that 3-Chip and 2-Chip SNES are just different names for the same exact model? Is that it?

3 chip and 2 chip refer to the same group of models

But they're different, right?

---

## #46 — maxtherabbit
*Sat Oct 03, 2020 2:37 pm*


> 

Lawfer wrote:
But they're different, right?

no, there is no such thing as a 2 chip

every SNES before the 1 chip is a 3 chip, but they are not all the same model

2 chip is a malapropism

---

## #47 — VajSkids Consoles
*Sat Oct 03, 2020 2:42 pm*


That sharp photo of Zelda there - drool worthy to get that out of original hardware ! I’d say *sold* if it weren’t for the PPU needing to be removed and replanted... that seems a bit excruciating unless you have the infrared bench

---

## #48 — Maka8295
*Sat Oct 03, 2020 2:50 pm*


> 

VajSkids Consoles wrote:That sharp photo of Zelda there - drool worthy to get that out of original hardware ! I’d say *sold* if it weren’t for the PPU needing to be removed and replanted... that seems a bit excruciating unless you have the infrared bench

You could probably take it off pretty easily with some low melting point solder or hot air.

---

## #49 — VajSkids Consoles
*Sat Oct 03, 2020 8:06 pm*


Yeah but then it needs a transplant. You’ve already seen how his originally transplanted one came out (this isn’t having a crack at you, that shits super difficult) 

But if you use one of those Benches used for reballing GPU’s etc it would be a sinch, got a cheap one off eBay years ago and it worked a treat. It does bursts of infrared rather than constant heat and the advantage is it seems to get the solder flowing without heating up the IC too much. 

To reseat it you just put some solder on the new boards footprint. Not a tonne just enough. Take some time getting it perfectly into place and then infrared it back on.

---

## #50 — Opatus
*Sat Oct 03, 2020 9:15 pm*


Removing the PPU is easy with hot air if you are careful. Soldering it on is even easier with a simple iron.

The damage did not happen during the removal of it. Before I've removed it I was probing the TST pins a lot and after some time a few pins have broken off. I was a bit careless because I was under the impression that the PPU2 was faulty (missing sprites), but most likely I was wrong.

---

## #51 — VajSkids Consoles
*Sun Oct 04, 2020 4:50 am*


> 

Opatus wrote:Removing the PPU is easy with hot air if you are careful. Soldering it on is even easier with a simple iron.

The damage did not happen during the removal of it. Before I've removed it I was probing the TST pins a lot and after some time a few pins have broken off. I was a bit careless because I was under the impression that the PPU2 was faulty (missing sprites), but most likely I was wrong.

No worries- been a while since I opened a SNES/ Famicom. I actually chose to use my super famicom over my one chip because it was a little bright of an output and I couldn’t be bothered opening it up to add the 3 x 75ohm resistors known as a fix. 

Anyway, I’d never attempt to using an iron on say, a megadrive VDP transplant.  Those pins are insanely close and brittle

---

## #52 — yveltalgriffin
*Mon Oct 05, 2020 6:19 pm*


This is awesome, Opatus! Can't wait to see the new board in action. I'd be interested in integrating your design into a SNES motherboard redesign I'm working on for a portable SNES handheld. I was originally making a new 1Chip board, but based off this work, a 3-Chip with digital video out is clearly superior. With a linedoubler and some extra logic implemented in the CPLD (or an fpga if the cpld can't handle it) you could drive a LCD directly and bypass the DAC and LCD driver board for emulator-quality video on original hardware.

---

## #53 — Opatus
*Tue Oct 13, 2020 3:22 pm*


If everything goes as planned I will open source the design. I'd like to see an HDMI mod as well, hopefully someone makes one.

---

## #54 — Opatus
*Wed Oct 14, 2020 7:37 am*


Yesterday I've finished assembling the new board. After a quick test the Mode 7 patch seems to be working and the video is clean and sharp. 

More results will follow. I'll try to get some direct video captures.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 2 - shmups.system11.org_files/6mjYIYf.jpg)

---

## #55 — maxtherabbit
*Wed Oct 14, 2020 12:34 pm*


> 

Opatus wrote:Yesterday I've finished assembling the new board. After a quick test the Mode 7 patch seems to be working and the video is clean and sharp. 

More results will follow. I'll try to get some direct video captures.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 2 - shmups.system11.org_files/6mjYIYf.jpg)

That looks awesome, how exactly does it attach to the mainboard?

---

## #56 — Kez
*Wed Oct 14, 2020 1:33 pm*


Very excited by this, I'd definitely be interested in trying it out!

> 

maxtherabbit wrote:That looks awesome, how exactly does it attach to the mainboard?

Looks as though PPU2 is replaced by a socket that the board attaches onto.

---

## #57 — maxtherabbit
*Wed Oct 14, 2020 1:55 pm*


> 

Kez wrote:Very excited by this, I'd definitely be interested in trying it out!

> 

maxtherabbit wrote:That looks awesome, how exactly does it attach to the mainboard?

Looks as though PPU2 is replaced by a socket that the board attaches onto.

Yeah I gathered that but what kind of socket attaches to a QFP footprint?

---

## #58 — Opatus
*Wed Oct 14, 2020 2:09 pm*


Here is an image of the previous board, the adatper PCB was not changed. I didn't want to lift 20+ pins so I made an adapter. It is soldered to the QFP footprint like a QFN chip would. Unfortunately no manufacturer could do 0.65 mm edge plating.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 2 - shmups.system11.org_files/gt7W9FK.jpg)

As I have only had a PAL SNES I made my mod board based on the dimensions of that mainboard. So it is only directly compatible with the original NTSC revision (SHVC-CPU-XX). But I've made a 2nd adapter board for the other 3 revisions(GPM, RGB, APU). If the mod board is rotated 180° in theory it will fit those. I did not try it yet.

---

## #59 — maxtherabbit
*Wed Oct 14, 2020 2:47 pm*


So the adapter board just has a QFP footprint of its own on the bottom and you have to line it up blind? Was that as bad as it sounds?

---

## #60 — Konsolkongen
*Wed Oct 14, 2020 2:57 pm*


At least it looks like it's easy to check for continuity and shorts.

---

## #61 — Opatus
*Wed Oct 14, 2020 3:47 pm*


I've presoldered both the footprint and the board, then I've reflowed the pins. Tried it on 2 mainboards, did not have any issues. It's easier than it sounds. And as there's a really small gap between the mobo and the adapter you can see if there's a connection.

Lining up is also no big deal if you take your time and check the pins. The new version has the pads on the top side as well, maybe it helps.

If you have a better solution, I'm open to it.

---

## #62 — Harrumph
*Wed Oct 14, 2020 7:00 pm*


Nice work!

---

## #63 — Opatus
*Wed Oct 14, 2020 10:15 pm*


Here are some examples of the Mode 7 patch in action. And a bonus image of Air Strike Patrol's infamous shadow. I couldn't do a direct capture because I couldn't get decent quality out of my cheap capture device.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/PujPkyk.jpg)

Spoiler

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/36T85iO.jpg)

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/ll1J0nH.jpg)

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/eYvR07a.jpg)

---

## #64 — yveltalgriffin
*Wed Oct 14, 2020 10:51 pm*


I like your mounting solution a lot. For a hypothetical future HDMI mod, maybe a FPC interposer between the PPU2 and mainboard could streamline things. That would enable a low profile install and a built-in FFC for connecting to the retiming board.

---

## #65 — Opatus
*Thu Oct 15, 2020 8:36 am*


To be fair I took the idea from the UltraPIF and then improvised a bit. I thought about using an FPC but I had no experience with it, costs more to prototype and I was not sure if the whole project would succeed. So I've dropped it.

---

## #66 — Konsolkongen
*Thu Oct 15, 2020 9:21 am*


I think what you have achieved here is really great. The older model SNES consoles are useless IMO because of the worst RGB image of any system. The easy solution has always been to get a 1chip but they are getting harder and more expensive to track down.

Would something like this be possible on the Mega Drive too? Not that its RGB image is anywhere near as bad as the SNES, but there are faint jailbars and unfortunately none of the bypass solutions make any difference, at least not on the revisions I own.

---

## #67 — Maka8295
*Thu Oct 15, 2020 9:37 am*


> 

Konsolkongen wrote:I think what you have achieved here is really great. The older model SNES consoles are useless IMO because of the worst RGB image of any system. The easy solution has always been to get a 1chip but they are getting harder and more expensive to track down.

Would something like this be possible on the Mega Drive too? Not that its RGB image is anywhere near as bad as the SNES, but there are faint jailbars and unfortunately none of the bypass solutions make any difference, at least not on the revisions I own.

I feel like the 1chips get too much praise, sure they have a sharp picture, but you have to put up with ghosting, glitches, overblown whites, and the annoying black line that appears in a lot of games (as well as some other issues). The colours also look washed out without an RGB bypass or the resistor mod.

---

## #68 — Opatus
*Thu Oct 15, 2020 9:46 am*


I don't think it is possible on the Mega Drive. I have a VA4 PAL Mega Drive, where I also couldn't completely get rid of the noise. I thought about running the RGB through an ADC and snapping the digitized signal to the next valid color. If I remember correctly the Mega Drive has a 0-5V video signal coming out of the VDP and it's only 3bit per line. So it would give us about +-0.35V margin for each color step. Which is plenty. If we wanted to go overboard we could compare the results again the CRAM to get a more precise result. But it would probably not be necessarily.

And if we are already using an FPGA for that we could correct the horribly timed Csync as well.

---

## #69 — Konsolkongen
*Thu Oct 15, 2020 9:47 am*


> 

Maka8295 wrote:
I feel like the 1chips get too much praise, sure they have a sharp picture, but you have to put up with ghosting, glitches, overblown whites, and the annoying black line that appears in a lot of games (as well as some other issues). The colours also look washed out without an RGB bypass or the resistor mod.

Could you point me towards an example of that black line you mention. Not sure I’m aware of that issue. As for the ghosting and blown out colors these issues are easily fixed with a few cheap components. As is the jailbar right down the middle that’s visible on dark screens. 

I don’t understand the need to bypass RGB on this system either as you can get pretty perfect results with the stock hardware. Unless I’ve missed something when playing, like that black line you mention ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/icon_smile.gif)

---

## #70 — Kez
*Thu Oct 15, 2020 9:52 am*


> 

Konsolkongen wrote:Could you point me towards an example of that black line you mention.

Street Fighter 2 Turbo is the example that springs to mind, there is a black line right at the top of the screen that runs partway across. The C11 ghosting fix changes the length of the line.

EDIT: [Here's a post about it](https://shmups.system11.org/viewtopic.php?p=1286695#p1286695)

---

## #71 — Maka8295
*Thu Oct 15, 2020 10:06 am*


> 

Konsolkongen wrote:

> 

Maka8295 wrote:
I feel like the 1chips get too much praise, sure they have a sharp picture, but you have to put up with ghosting, glitches, overblown whites, and the annoying black line that appears in a lot of games (as well as some other issues). The colours also look washed out without an RGB bypass or the resistor mod.

Could you point me towards an example of that black line you mention. Not sure I’m aware of that issue. As for the ghosting and blown out colors these issues are easily fixed with a few cheap components. As is the jailbar right down the middle that’s visible on dark screens. 

I don’t understand the need to bypass RGB on this system either as you can get pretty perfect results with the stock hardware. Unless I’ve missed something when playing, like that black line you mention ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/icon_smile.gif)

Heres a good article talking about the black line ([https://retromods.game.blog/snes-sfc/sn ... sting-fix/](https://retromods.game.blog/snes-sfc/snes-1-chip-ghosting-fix/))

Some games I've noticed that have the black line:

Link to the past (when opening the item menu or transitioning worlds)

Chrono Trigger (when opening the menu, this one is pretty minor)

Mario Collection Mario 3 (Just above the HUD info thing and on the top of the screen during water levels when pausing the game)

Yoshi's Island (2 lines of pixels are missing from the top of the screen, with a third line very dim with the C11 fix, its just a single faded line of pixels without)

Front Mission Gun Hazard (same as yoshi's island except it actually partially covers some important HUD info)

Tactics Ogre (Top line of pixels on the HUD flickers even without C11, I imagine this one could be very problematic with the C11 fix as its right above some important info and appears right in the middle of the screen.)

Super Gameboy (top line of pixels flickers pretty badly, but you can just set the border to black to get rid of it)

Street fighter (as someone else has mentioned)

But the most annoying problem I've had, is a weird sort of interference that 1chips seem to have that I've never seen anyone else talk about, bright colours leave a trail on dark backgrounds (different ghosting) and theres a strange flickery noise outside the game window which sometimes encroaches into the game screen itself. Luckily the OSSC can fix all of it except for the trails. All 3 of my 1chips have this problem and none of my 3/2 chips have it.

---

## #72 — Konsolkongen
*Thu Oct 15, 2020 10:44 am*


> 

Opatus wrote:I don't think it is possible on the Mega Drive. I have a VA4 PAL Mega Drive, where I also couldn't completely get rid of the noise. I thought about running the RGB through an ADC and snapping the digitized signal to the next valid color. If I remember correctly the Mega Drive has a 0-5V video signal coming out of the VDP and it's only 3bit per line. So it would give us about +-0.35V margin for each color step. Which is plenty. If we wanted to go overboard we could compare the results again the CRAM to get a more precise result. But it would probably not be necessarily.

And if we are already using an FPGA for that we could correct the horribly timed Csync as well.

Doesn't sound that complicated. Not sure about the 0-5V but it seems logical. I can check on mine if you'd like. Would be really great if it could be solved eventually ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/icon_smile.gif)

Kez & Maka8295>

Thank you. I remember that black line now. It's annoying but I'd take that over the ghosting any day :/

---

## #73 — yoshiyukiblade
*Thu Oct 15, 2020 3:23 pm*


Amazing results! So it looks like the known issues, discussed in this thread so far, were resolved. Let's hope that there aren't any more nasty surprises lurking around. ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/icon_smile.gif)

---

## #74 — paulb_nl
*Thu Oct 15, 2020 6:20 pm*


> 

Maka8295 wrote:
But the most annoying problem I've had, is a weird sort of interference that 1chips seem to have that I've never seen anyone else talk about, bright colours leave a trail on dark backgrounds (different ghosting) and theres a strange flickery noise outside the game window which sometimes encroaches into the game screen itself. Luckily the OSSC can fix all of it except for the trails. All 3 of my 1chips have this problem and none of my 3/2 chips have it.

It sounds like a problem with your OSSC instead of the 1-CHIPs. I have not seen those issues with my three 1-CHIPs and SNES jr.

An increased brightness outside of the game window usually indicates a defective OSSC unless there is something wrong with your RGB cable.

---

## #75 — Unseen
*Thu Oct 15, 2020 8:18 pm*


> 

Konsolkongen wrote:Would something like this be possible on the Mega Drive too?

IIRC: The graphics chip in the Mega Drive 1 has an unused port that can output the palette index of the current pixel that could be used to create a high-quality reconstruction of the output image. However, that port must be enabled in software first and tracking the current palette to convert that index to a color is a bit complicated because the palette can be manipulated using DMA.

---

## #76 — Opatus
*Fri Oct 16, 2020 9:20 am*


> 

yoshiyukiblade wrote:Amazing results! So it looks like the known issues, discussed in this thread so far, were resolved. Let's hope that there aren't any more nasty surprises lurking around. ![:)](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/icon_smile.gif)

I'm sure there will be surprises, there always are. Did you make your board with the simple DAC that you were planning?

---

## #77 — yoshiyukiblade
*Fri Oct 16, 2020 10:25 pm*


Yeah, I designed the board and had it made a couple months ago, but decided not to install it for now. The thought of hand soldering 15 tiny wires was rather intimidating! This was before I thought about designing an adapter board like your prototypes. I think I'll "finish" the analog RLPF project first before playing around with digital stuff more.

I want to see if it's possible to eliminate the high frequency ripple seen in the analog signal by feeding *all* the PPU2 VCC pins with a low noise source. If not, I think it's safe to assume that all that ripple is generated inside the PPU itself or the data inputs are contributing significant amounts of interference seen at the output.

---

## #78 — mikejmoffitt
*Sat Oct 17, 2020 12:29 am*


This mod looks great, I look forward to the final results.

As for the DAC: I recommend the ADV7123, as it is a 10-bit parallel DAC - double what you need. So, it's trivial to see how you'd double up bits without introducing a tiny disturbance in the ramp. Same pinout, just some N/C pins become additional LSBs.

Regarding Megadrive: A similar thing is possible, but the VDP only outputs digitally the pixel data, palette index, and bg/spr select. Those are intended to form the lookup address within external Color RAM. In that mode, the internal CRAM is disabled. This is how the System C/C2 works. In a sense, this is similar to how NESRGB works.

Unfortunately, making a Megadrive solution would necessitate snooping all writes to the internal color RAM, which is done through the VDP. The easiest ones would be simple port read/writes, but most games use DMA to transfer palette lines much faster. To snoop those would require a much tighter understanding and implementation of the MD VDP's DMA timings, which are influenced by its internal state heavily. That is the part that makes that mod seem too challenging.

---

## #79 — paulb_nl
*Sat Oct 17, 2020 10:28 am*


> 

mikejmoffitt wrote:As for the DAC: I recommend the ADV7123, as it is a 10-bit parallel DAC - double what you need. So, it's trivial to see how you'd double up bits without introducing a tiny disturbance in the ramp. Same pinout, just some N/C pins become additional LSBs.

The SNES has a 4 bit brightness level and 5 bits per color so 9 bits are needed to show all possible colors.

---

## #80 — Opatus
*Sat Oct 17, 2020 12:45 pm*


@yoshiyukiblade

Too bad, I wanted to see someone's results as well.

I don't think the high frequencies are coming from the PPU, you're introducing a lot of really high frequencies with that slew rate boost on the video signal. Maybe you could try an even faster opamp, but I don't know. 

@mikejmoffitt

I already use the ADV7123, because as paulb_nl said I have almost 9 full bits of video per channel. If I've calculated everything correctly, I have a 0.7V linear output. But I didn't check it, first I was more interested in the bigger picture, namely the Mode 7 patch.

I will experiment with the Mega Drive, when I find the time for it. But first I have still plans for this SNES mod.

---

## #81 — 6t8k
*Sat Oct 17, 2020 5:44 pm*


That's great progress.

With an FPC connection between the original PPU2 footprint and the mod board, it may be possible to fit the mod board somewhere else in case this becomes a concern with different main board versions, or a new connector needs to be incorporated into the side of the casing. But I also really like your current solution.

---

## #82 — maxtherabbit
*Sat Oct 17, 2020 6:49 pm*


As apprehensive as I am about lining up the QFP adapter board, I imagine it makes for an excellent mounting platform, mechanically.

---

## #83 — qjkxbmwvz
*Sat Oct 17, 2020 10:21 pm*


> 

paulb_nl wrote:

> 

mikejmoffitt wrote:As for the DAC: I recommend the ADV7123, as it is a 10-bit parallel DAC - double what you need. So, it's trivial to see how you'd double up bits without introducing a tiny disturbance in the ramp. Same pinout, just some N/C pins become additional LSBs.

The SNES has a 4 bit brightness level and 5 bits per color so 9 bits are needed to show all possible colors.

Does the real SNES actually have 512 brightness levels per channel?  If you were to set the minimal 15-bit color (1,1,1) and then vary the brightness bits, would there actually be fourteen different shades between min RGB @ max brightness and black?

---

## #84 — yoshiyukiblade
*Sat Oct 17, 2020 11:13 pm*


If I understood it correctly, stock DAC outputs the 5-bit steps (though technically it's continuous because it's analog), and the brightness register alters the voltage reference independently, attenuating the brightness like a digital volume knob. So for each step in the 5-bit output, there are 16 steps of brightness they can take.

There are some overlapping/redundant values though. Like if the brightness level is zero, any 5-bit output is zero. And if the 5-bit output is zero, any brightness value also yields zero.

---

## #85 — paulb_nl
*Sat Oct 17, 2020 11:26 pm*


> 

qjkxbmwvz wrote:Does the real SNES actually have 512 brightness levels per channel?  If you were to set the minimal 15-bit color (1,1,1) and then vary the brightness bits, would there actually be fourteen different shades between min RGB @ max brightness and black?

Well in theory yes but color(1,1,1) should be around 22mV and once you go into noise floor levels then it is difficult to measure.

[This post](http://forums.nesdev.com/viewtopic.php?p=257831#p257831) with measurements on nesdev mentions there is about 7-12mV of noise on his SNES. Also brightness level 0 seems to be just above 0mV with color 31,31,31.

---

## #86 — Opatus
*Sun Oct 18, 2020 8:09 pm*


> 

This post with measurements on nesdev mentions there is about 7-12mV of noise on his SNES. Also brightness level 0 seems to be just above 0mV with color 31,31,31.

This is interesting. As of right now, I just use the brightness register linearly, but I may think about making it non-linear. Even if the non-linearity is probably not noticeable at all.

---

## #87 — ikari_01
*Sat Oct 24, 2020 6:42 am*


FWIW I took some measurements of my PPU2 analog outputs back when tinkering around with the TST pins:

[https://drive.google.com/drive/folders/ ... sp=sharing](https://drive.google.com/drive/folders/1OOemH34zLlsu0oYd1N_xKM-3mFT2NTC6?usp=sharing)

Congratulations on the OVER pullup discovery, that had been pretty much the show stopper for Unseen and me back then.

I'm curious what the output of this ROM looks like with your mod, especially with the "OVER2" test ROM:

[https://sd2snes.de/files/misc/sppu-digi ... essdemo.7z](https://sd2snes.de/files/misc/sppu-digital/krom-chessdemo.7z)

It is a Mode 7 test written by krom that shows a checkerboard on a plane with some color math and sprite priority variations.

You can move around the checkerboard using the D-pad.

I have prepared "OVER0", "OVER2", and "OVER3" ROMs which correspond to the values of the OVER bits in register $211a (bits 7+6).

OVER0 repeats the checkerboard over and over, OVER2 shows a single board over the backdrop, and OVER3 repeats tile 0 around the checkerboard (so there is a single board on an infinite plane).

I don't expect any trouble with OVER0 and OVER3. Here's a reference shot for OVER2:

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/Chess-OVER2.png)

---

## #88 — Opatus
*Sat Oct 24, 2020 7:57 pm*


Unfortunately I don't have a flash cart, so I cannot run it. But some people were interested in the mod and they wanted to try it. They should have their boards ready in about 2 weeks. I think at least one of them will have a cart and hopefully they can test it.

I assume that it will work fine, but confirmation is needed. The way I do the Mode 7 now is making the PPU2 think it has still not left the active area by pulling the OVER signal high and injecting Color #0 from the VRAM. So the priority and color math should not be effected.

It's good to see that the signal from the PPU2 directly is fairly linear and the non-lineraity comes from the transistors on the motherboard.

And I also like to see the peak voltage of the video signal being 2.43V. My board has 2 options (technically 4) to route the RGB. One is a direct RGB output that has to be connected to the Multiout. The other is using the amplifier from the motherboard to clean up compostie and S-Video. For that I've calculated my amplifiers to generate 2.5V, I didn't have a pure white image to measure it and after a quick measurement 2.5V seemed reasonable.

---

## #89 — Opatus
*Thu Jan 14, 2021 11:46 am*


I bought an SD2SNES during Black Friday, which has finally arrived (thank you ikari_01 for creating it).

So I had the chance to try the chess table and as I have expected seems to be fine.

Here is a video and an image:

https://youtu.be/n3JoIReIQJw

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 3 - shmups.system11.org_files/7iIldbW.jpg)

Please note that there is some color inaccuracy and blurriness. I rewired my SNES to use the original video encoder but this way the green line picks up some noise from the blue digital lines, causing some problems. 

I ran out of space on my mod board and the green line had to be routed directly under the video DAC.

---

## #90 — gordon-creAtive
*Fri Jan 15, 2021 7:22 pm*


Are you guys aware of the [RGBtoHDMI](https://github.com/hoglet67/RGBtoHDMI/wiki) project? Could this help with an HDMI output?

---

## #91 — Pal360
*Sun Apr 04, 2021 10:01 am*


Any news Opatus on how the project is coming along? ![:D](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/icon_biggrin.gif)

---

## #92 — Opatus
*Fri Apr 30, 2021 3:12 pm*


> 

Pal360 wrote:Any news Opatus on how the project is coming along? ![:D](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/icon_biggrin.gif)

Unfortunately the project was on hold since Christmas. I will try to find some time to continue working on it.

---

## #93 — RGB0b
*Tue May 04, 2021 1:35 pm*


I had committed to helping and completely dropped the ball ![:(](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/icon_sad.gif)  Between the package delayed for months in shipping, getting sick and keeping up with the day-to-day, I just totally failed.  If anyone wants to help, please DM me a Digikey cart that I can just click on and order, then I'll get the final parts.  I'm really, *really* sorry for the delay.

---

## #94 — ikari_01
*Wed Jun 02, 2021 7:24 am*


> 

Opatus wrote:
Here is a video and an image:

https://youtu.be/n3JoIReIQJw

Yeah, that looks flawless. Awesome job!  ![8)](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/icon_cool.gif)

---

## #95 — that1crzywhtguy
*Sun Jul 25, 2021 5:56 pm*


![:shock:](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/icon_eek.gif)

---

## #96 — NewSchoolBoxer
*Sun Jul 25, 2021 10:31 pm*


3-Chip isn't a common term. I get that it's a rare version of the 1-Chip SNES but, unlike other 1-Chips, has no Csync before modding.

Is 3-Chip in the SNES Jr. form only or is it in the normal form factor with native RGB?

Sorry if I missed it in the thread, but does the board have a Bill of Materials or is it closed source? Not against closed source for business reasons. I'm just suspicious of another SNES mod's addition with no proof that it does anything and the chip looks like a Chinese counterfeit.

Still surprises me how little testing or validation of a board is needed to get people to install it in their consoles. First version of NES and N64 RGB mods had flaws that got fixed in the next version. Pre-H revisions of SD2SNES have an audio flaw.

---

## #97 — maxtherabbit
*Sun Jul 25, 2021 10:45 pm*


3 chip refers to all SNES / SFC revisions prior to the 1CHIP

CPU 

PPU-1

PPU-2

3 chips

---

## #98 — Kez
*Mon Jul 26, 2021 9:41 am*


> 

NewSchoolBoxer wrote:3-Chip isn't a common term. I get that it's a rare version of the 1-Chip SNES but, unlike other 1-Chips, has no Csync before modding.

Is 3-Chip in the SNES Jr. form only or is it in the normal form factor with native RGB?

Sorry if I missed it in the thread, but does the board have a Bill of Materials or is it closed source? Not against closed source for business reasons. I'm just suspicious of another SNES mod's addition with no proof that it does anything and the chip looks like a Chinese counterfeit.

Still surprises me how little testing or validation of a board is needed to get people to install it in their consoles. First version of NES and N64 RGB mods had flaws that got fixed in the next version. Pre-H revisions of SD2SNES have an audio flaw.

Completion and widespread availability of this mod would be a total gamechanger for the SNES, it's a huge deal really. It would mean that pretty much any SNES can match the video output quality of the 1CHIP with none of the drawbacks. No more playing lottery with serial numbers on eBay, just get any old SNES and install this mod. It also lays a strong foundation for future HDMI mods.

I'm not sure your suspicions have any basis in reality tbh, as OP has had long technical discussions with a bunch of established and respected developers in this thread, it's obvious they know what they are doing.

Of course there may be some bumps in the road for early adopters, but that's just how it goes and it all just contributes to a better mod and more community knowledge in the long run.

---

## #99 — Opatus
*Mon Jul 26, 2021 3:31 pm*


> 

NewSchoolBoxer wrote:Sorry if I missed it in the thread, but does the board have a Bill of Materials or is it closed source? Not against closed source for business reasons. I'm just suspicious of another SNES mod's addition with no proof that it does anything and the chip looks like a Chinese counterfeit.

Still surprises me how little testing or validation of a board is needed to get people to install it in their consoles. First version of NES and N64 RGB mods had flaws that got fixed in the next version. Pre-H revisions of SD2SNES have an audio flaw.

The project won't be closed source, worst case it will be never quite ready and won't be open either. The basics of this project have been already investigated a few years ago. The point of my project was to further research and try to find a solution for the problems that were found. Progress has been very slow lately, I had absolute no time for it.

I'm wondering which chip do you refer to by "Chinese counterfeit". There's a PPU2 from a 3-Chip SNES (PAL) and an Altera CPLD. You should also understand that most mods are made by one person and not a team with limited resources, some mistakes are mostly inevitable. Even mainstream commercial products go through revisions, correcting some early mistakes. So I don't really get where your suspicion comes from. I also believe that for all your examples a solution was made available for the early adopters.

---

## #100 — yoshiyukiblade
*Tue Jul 27, 2021 5:56 pm*


I was thinking about this project again recently and wondered if there was any news about it. Have any new issues cropped up since your most recent prototype(s)? I might take another stab at this to help out where I can. Maybe play-testing and video captures if nothing else.

Inspired by this project, I successfully designed and installed a similar PCB adapter for my reverse LPF project, so I'm a little more confident in making something for the digital RGB outputs again. My SNES mobo is an NTSC GPM-02 unit, so all the chips are crammed close together and there is little clearance between S-PPU2 and the cartridge connector. I had to offset the position of S-PPU2 on the mod board, relative to its original footprint on the mobo, to get some decent clearance around pins 81-100. It was kind of an awkward design, but worked out well enough.

Is it possible to implement the mode7 fix easily? I got kinda lost in understanding how you accomplished it. If it can be done with some discrete logic gates/inverters, that would be nice.

From what I remember, some of our video output results differed and I didn't see a lot of the issues you described. I wonder if it's related to the S-PPU2 revision (I have revision C), or the region (NTSC vs PAL). In any case, everything I saw on the analog output looked good (except for the mode 7 graphical glitches I reported) when I did the following modifications:

- TST15 (pin 93) tied to VCC

- TST12, TEST13, and TST14 (pins 90, 91, and 92) floating

- OVER1 and OVER2 (pins 37 and 50) tied to VCC

I'll need to dust off the test unit to double-check for any other graphical problems.

---

## #101 — NewSchoolBoxer
*Thu Jul 29, 2021 1:36 pm*


> 

maxtherabbit wrote:3 chip refers to all SNES / SFC revisions prior to the 1CHIP

CPU 

PPU-1

PPU-2

3 chips

I see, so in German language discussion, they call the 2CHIP the 3CHIP. I couldn't find a reference to what a 3CHIP was so assumed it was related to the rare 1CHIP-03 revision. Marketing a mod to the US and Canada, my advice is calling it the 2CHIP like in the popular (English) RetroRGB and My Life in Gaming content. Not saying English terms are better, we got the "composite sync" vs "composite video as sync" mess.

> 

Kez wrote:

> 

NewSchoolBoxer wrote:3-Chip isn't a common term. I get that it's a rare version of the 1-Chip SNES but, unlike other 1-Chips, has no Csync before modding.

Is 3-Chip in the SNES Jr. form only or is it in the normal form factor with native RGB?

Sorry if I missed it in the thread, but does the board have a Bill of Materials or is it closed source? Not against closed source for business reasons. I'm just suspicious of another SNES mod's addition with no proof that it does anything and the chip looks like a Chinese counterfeit.

Still surprises me how little testing or validation of a board is needed to get people to install it in their consoles. First version of NES and N64 RGB mods had flaws that got fixed in the next version. Pre-H revisions of SD2SNES have an audio flaw.

Completion and widespread availability of this mod would be a total gamechanger for the SNES, it's a huge deal really. It would mean that pretty much any SNES can match the video output quality of the 1CHIP with none of the drawbacks. No more playing lottery with serial numbers on eBay, just get any old SNES and install this mod. It also lays a strong foundation for future HDMI mods.

I'm not sure your suspicions have any basis in reality tbh, as OP has had long technical discussions with a bunch of established and respected developers in this thread, it's obvious they know what they are doing.

Of course there may be some bumps in the road for early adopters, but that's just how it goes and it all just contributes to a better mod and more community knowledge in the long run.

I'm sorry, do you have modder friends who made design mistakes without performing due diligence that costumers paid another $50 + shipping to fix? Is being an expert modder the same as being an expert electrician or knowing you're supposed to [terminate video with 75 ohm resistors](https://shmups.system11.org/viewtopic.php?f=6&t=55948)?

This is a gamechanger? I bought a 1CHIP-02 SFC from Japan and I can't tell a difference in video quality from my 2CHIP just eyeballing on an L2. So we're going to perma destroy SNES consoles by removing their PPUs instead of fix them and put said chips on PCBs with FPGAs? I totally get that soldering the pins of a chip no longer made is too risky but how long is the aged PPU from a broken console going to last being run off-spec? After it dies, does the customer pays another $50 + shipping to revert back to umodded state?

I only mention FPGAs because they're expensive. Is this mod to cost less than the $90 / €76 I paid for 1CHIP + power supply? You know why I look over mods even though I don't do them? I credit Voultar and RetroRGB for [educating modders](https://shmups.system11.org/viewtopic.php?t=58863) to switch from THS7314 to THS7343 in 2015-2016. I joined this scene in 2019 so I had to imagine one person using the 14 then everyone copying the first solution without actually checking what other TI chips existed.

Meanwhile, every junior in electrical engineering learns not to chain two LPFs together without either calculating, simulating or measuring what the cutoff frequency reduces to. Phase and group delay would be considered, as well as ripples in the passband and the order of the filters for stability or load concerns. I'm not trying to sound opaque, these are fundamental topics.

> 

Opatus wrote:
The project won't be closed source, worst case it will be never quite ready and won't be open either. The basics of this project have been already investigated a few years ago. The point of my project was to further research and try to find a solution for the problems that were found. Progress has been very slow lately, I had absolute no time for it.

I'm wondering which chip do you refer to by "Chinese counterfeit". There's a PPU2 from a 3-Chip SNES (PAL) and an Altera CPLD. You should also understand that most mods are made by one person and not a team with limited resources, some mistakes are mostly inevitable. Even mainstream commercial products go through revisions, correcting some early mistakes. So I don't really get where your suspicion comes from. I also believe that for all your examples a solution was made available for the early adopters.

I read through the https://circuit-board.de/ thread in rough machine translation. I didn't realize you had ikari, borti and rama in the scene. Real heavyweights.

Sorry, I didn't mean to allude to [counterfeit chips](https://archive.is/TXkWH) applying to you because they don't. I'm fascinated by the technical detail and dedication on display for my favorite console. I believe in progress even if I disagree with end product. I should be helpful then.

You have to consider the electrical noise of the FPGA with its high switching speed and complexity. Could be causing much of your existing problems. I would not have placed it right next to the PPU and the PCB going under the steel EMI shield (or forcing its removal?) is unfortunate. Aluminum and copper EMI shields are better but cost more.

---

## #102 — maxtherabbit
*Thu Jul 29, 2021 2:39 pm*


> 

NewSchoolBoxer wrote:

> 

maxtherabbit wrote:3 chip refers to all SNES / SFC revisions prior to the 1CHIP

CPU 

PPU-1

PPU-2

3 chips

I see, so in German language discussion, they call the 2CHIP the 3CHIP. I couldn't find a reference to what a 3CHIP was so assumed it was related to the rare 1CHIP-03 revision. Marketing a mod to the US and Canada, my advice is calling it the 2CHIP like in the popular (English) RetroRGB and My Life in Gaming content. Not saying English terms are better, we got the "composite sync" vs "composite video as sync" mess.

What? I'm not german. The people who call them 2-chip simply can't count

---

## #103 — Kez
*Thu Jul 29, 2021 4:25 pm*


> 

NewSchoolBoxer wrote:I'm sorry, do you have modder friends who made design mistakes without performing due diligence that costumers paid another $50 + shipping to fix? Is being an expert modder the same as being an expert electrician or knowing you're supposed to [terminate video with 75 ohm resistors](https://shmups.system11.org/viewtopic.php?f=6&t=55948)?

This is a gamechanger? I bought a 1CHIP-02 SFC from Japan and I can't tell a difference in video quality from my 2CHIP just eyeballing on an L2. So we're going to perma destroy SNES consoles by removing their PPUs instead of fix them and put said chips on PCBs with FPGAs? I totally get that soldering the pins of a chip no longer made is too risky but how long is the aged PPU from a broken console going to last being run off-spec? After it dies, does the customer pays another $50 + shipping to revert back to umodded state?

I only mention FPGAs because they're expensive. Is this mod to cost less than the $90 / €76 I paid for 1CHIP + power supply? You know why I look over mods even though I don't do them? I credit Voultar and RetroRGB for [educating modders](https://shmups.system11.org/viewtopic.php?t=58863) to switch from THS7314 to THS7343 in 2015-2016. I joined this scene in 2019 so I had to imagine one person using the 14 then everyone copying the first solution without actually checking what other TI chips existed.

Meanwhile, every junior in electrical engineering learns not to chain two LPFs together without either calculating, simulating or measuring what the cutoff frequency reduces to. Phase and group delay would be considered, as well as ripples in the passband and the order of the filters for stability or load concerns. I'm not trying to sound opaque, these are fundamental topics.

I'm not really sure what kind of point you're trying to make here. The 1CHIP SNES is way sharper than the older models, that is extremely well established. Congrats on getting one for less than half the going rate, shall we make a quick checklist of all the stuff you don't need and put the word out for everyone to abandon any related hobby projects?

Nobody is trying to sell this as a mod, it's basically a proof of concept at this point. They are just experimenting with the SNES and talking about the results. The community is built on the shoulders of people who do work like this in their free time, expecting very little in return. Your attitude just doesn't match this situation at all.

---

## #104 — Opatus
*Fri Jul 30, 2021 10:03 pm*


@yoshiyukiblade

> 

Is it possible to implement the mode7 fix easily? I got kinda lost in understanding how you accomplished it. If it can be done with some discrete logic gates/inverters, that would be nice.

From what I remember, some of our video output results differed and I didn't see a lot of the issues you described. I wonder if it's related to the S-PPU2 revision (I have revision C), or the region (NTSC vs PAL). In any case, everything I saw on the analog output looked good (except for the mode 7 graphical glitches I reported) when I did the following modifications:

It's nice reading that you got inspired. For the newer NTSC revisions I have created a different adapter board, with it it should be possible to fit the mod board upside down. But it was never actually tested.

I don't have any graphical issues anymore, at least I haven't seen any lately. Last time I have tied TST15 to VCC and I still didn't have any issues, but maybe at that time the PPU was running in NTSC mode.

You could do the mode7 fix with discrete logic. You have to check with the logic if you are in the problematic mode7 sub-mode. And then you have to pull down the data on the  B data bus of the PPU2 if the OVER signal is 0. For the data on the B bus I use a SN74HCT541PW with pull-down resistors.

Here how it's done in the FPGA:

wire screen_over_data = DATA[7] & !DATA[6];

wire mode_data = DATA[0] & DATA[1] & DATA[2];

assign MD7PAT = over & !OVER1;

assign over = mode & screen_over;

if(!PAWR && (PADDRESS [7:0] == 8'b00000101)) mode <= mode_data;

if(!PAWR && (PADDRESS [7:0] == 8'b00011010)) screen_over <= screen_over_data;

@NewSchoolBoxer

> 

I see, so in German language discussion, they call the 2CHIP the 3CHIP. I couldn't find a reference to what a 3CHIP was so assumed it was related to the rare 1CHIP-03 revision. Marketing a mod to the US and Canada, my advice is calling it the 2CHIP like in the popular (English) RetroRGB and My Life in Gaming content. Not saying English terms are better, we got the "composite sync" vs "composite video as sync" mess.

Calling it 3-chip makes more sense as the 1-chip SNES combines 3 chips. But calling it 2-chip is also acceptable because the PPU1 and PPU2 can be considered as 1 chip.

> 

This is a gamechanger? I bought a 1CHIP-02 SFC from Japan and I can't tell a difference in video quality from my 2CHIP just eyeballing on an L2. So we're going to perma destroy SNES consoles by removing their PPUs instead of fix them and put said chips on PCBs with FPGAs? I totally get that soldering the pins of a chip no longer made is too risky but how long is the aged PPU from a broken console going to last being run off-spec? After it dies, does the customer pays another $50 + shipping to revert back to umodded state?

I only mention FPGAs because they're expensive. Is this mod to cost less than the $90 / €76 I paid for 1CHIP + power supply? You know why I look over mods even though I don't do them? I credit Voultar and RetroRGB for educating modders to switch from THS7314 to THS7343 in 2015-2016. I joined this scene in 2019 so I had to imagine one person using the 14 then everyone copying the first solution without actually checking what other TI chips existed.

The video quality of the 1-chip SNES is so much better. I can clearly see it on my 14-M4 with RGB and on a 4k flat panel it looks awful.

As for the price I think BOM cost should be around $60 without assembly and installation. The FPGA/CPLD used here should be around $10 (but I'm not entirely sure if all the planned features would fit), of course it is out of stock right now.

---

## #105 — yoshiyukiblade
*Tue Aug 03, 2021 10:33 am*


> 

Opatus wrote:
You could do the mode7 fix with discrete logic. You have to check with the logic if you are in the problematic mode7 sub-mode. And then you have to pull down the data on the  B data bus of the PPU2 if the OVER signal is 0. For the data on the B bus I use a SN74HCT541PW with pull-down resistors.

I was wondering if we can naïvely apply an AND gate to OVER and VDB and connect the output to the VDB pins. I poked at the OVER signal and saw that it is normally held high until mode7 related content shows up (it is very active during the problem scenes). So when OVER is high, the output will just follow whatever VDB is doing. When OVER is low, the output will be low. I've never dealt with logic stuff, so let me know if this is a dumb idea or not  ![:P](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/icon_razz.gif).

---

## #106 — Opatus
*Tue Aug 03, 2021 2:48 pm*


The SNES has 3 options in what to do if the mode7 layer is zoomed out and thus it does not cover the whole display (see register 211A). It can either repeat the mode 7 "map", draw the base color (backdrop color) or draw the first entry in the palette. If I remember correctly the PPU1 calculates the repeated mode 7 map in every modes, but keeps the OVER signal high for the first mode (mode 7 repetition). The OVER signal (when driven low) is used by the PPU1 to tell the PPU2 in the latter 2 modes that the processed pixel is outside of the memory map. Than the PPU2 decides based on the register setting to draw the base color or the first palette entry. But if TST15 is pulled high the PPU2 won't display the base color correctly outside the mode 7 area.

So I believe you cannot use the OVER directly to control the data on VDB0..VDB7. You most likely don't have to check for mode 7 (the OVER signal can only change in mode 7) but you definitely have to evaulate register 211A. And this is not that difficult by using some logic.

---

## #107 — yoshiyukiblade
*Tue Aug 03, 2021 4:03 pm*


Ah, okay. This is definitely outside my current skill level as I have no experience in probing the registers to get the necessary information. I'll start with something simple in the meantime and work my way up from there. I'll have to learn about it sooner or later to get the brightness information as well. Thanks for the detailed description on how mode 7 operates and the role of OVER in these situations.

The current plan is to get an output that looks pretty and capture it!

---

## #108 — Opatus
*Sat Oct 16, 2021 1:37 pm*


I made the PCB files public (source, bom and gerbers): https://github.com/Opatusos/SNES_TST

Recently I made a lot of small changes and felt like I can share the project. These changes are unproven and might cause some problems, although they shouldn't. Unfortunately due to the parts shortage some parts of the BOM are really hard to obtain.

The included firmware is only a test firmware and nowhere near finished, as I currently do not have the time to develop it. I'm not even sure if this version is 100% compatible with the new PCB version.

---

## #109 — thchardcore
*Sat Oct 16, 2021 6:48 pm*


Super cool. Thank you!

---

## #110 — yoshiyukiblade
*Sun Oct 17, 2021 12:43 am*


Awesome! I hope someone with the knowledge and skills can test it out.

I've been facing parts supply issues for this hobby as well, even though my design is simple as dirt. Got the PCBs sitting here collecting dust, heh.

---

## #111 — Opatus
*Wed Nov 17, 2021 9:12 am*


I've assembled a new version of the board, where the layout of the video signals (which go directly to the SNES motherboard) was modified.

Here is an image for comparison between the generated and the original PPU video. All the settings and video amplifiers are identical, only the video source is different. The photos were taken using an OSSC in Line5x mode. The sampling is set to generic, with the low pass filter turned off to show the rise and fall time difference. Unfortunately the ADC gain of the OSSC was set to 11, I forgot to set it back to the default 8 before making the photos. So bright details might be crushed. As the LPF is turned off there's significant noise on all the typical SNES frequencies. With the LPF changed to 9.5 MHz, there's a surprisingly low noise, barely noticeable. The motherboard is SNSP-CPU-01 (PAL) and the input stage of the video amplifier (which is a PNP Darlington transistor) was changed to a simple PNP transistor. The original transistor had about 500 pF emitter capacitance, which resulted in slow signal rise time.

The other video output of the board (which can be directly connected to 75 Ohm) has practically no noise.

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/Lfjwtn3.png)

---

## #112 — Harrumph
*Thu Nov 18, 2021 9:04 pm*


I’m glad you are still working on this!

---

## #113 — bobrocks95
*Thu Nov 18, 2021 9:35 pm*


Would love to be able to sell my Junior one day and switch back to a nice chunky original model with that giant eject lever...

---

## #114 — Fluor
*Mon Jan 03, 2022 4:43 pm*


Thank you for continuing working on this. Will keep following it closely.

Snes is my favorite console and would love to have it modded just like the N64 with RGB/YPbPr + HDMI output.

---

## #115 — yoshiyukiblade
*Tue Jan 18, 2022 1:33 pm*


@Opatus

Looks great, as expected. You really need a proper capture setup to really show off the quality!

On a related note, I wanted to compare the unmodified PPU2 output from my mod board (all the reverse LPF stuff removed). It looks a bit cleaner than the one in your photo, though it's hard to compare them exactly.

Spoiler

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/uczZTbE.png)
On the OSSC, I used generic mode, upped the H sample rate to 8 * 341 and the active area to 1920 (needed to be 2048 to capture the whole frame, but the OSSC maxes out at 1920). The SDTV LPF was on. My capture device only does YUY2, so the chroma resolution was halved. The final image was padded and resized to maintain the correct aspect ratio.

Using optimal sampling:
Spoiler

![Image](./Sharp analog RGB for the 3-Chip SNES using digital signals - Page 4 - shmups.system11.org_files/rKpKPdl.png)
My guess is that parasitic capacitances greatly affect the unbuffered signal. During the board design, I put a bit of extra care in minimizing parasitic capacitances on the unbuffered pins by removing the ground planes below the pads and traces that connect to the base of the PNP transistors. The traces lengths were also less than 1 mm long, so this is about as clean as it gets for now. The remaining capacitance is probably intrinsic to the DAC architecture and IC package.

---

## #116 — Aquamentus
*Tue Jan 18, 2022 8:00 pm*


This is so sweet. Can’t wait for a complete kit one day - would love to use my SFC with Satellaview as my main SNES instead of the Jr.

---

## #117 — incrediblehark
*Sat Feb 25, 2023 3:35 pm*


Looks like someone had success with this mod:

[https://amaiorano.io/2022/10/14/snes-2- ... r-mod.html](https://amaiorano.io/2022/10/14/snes-2-chip-rgb-filter-mod.html)

I'm assuming this is a user on here, results look good! If anyone out there would be willing to perform this service for a fee I would be more than happy to send some cash their way!

---

## #118 — Kez
*Sat Feb 25, 2023 9:52 pm*


> 

incrediblehark wrote:Looks like someone had success with this mod:

[https://amaiorano.io/2022/10/14/snes-2- ... r-mod.html](https://amaiorano.io/2022/10/14/snes-2-chip-rgb-filter-mod.html)

I'm assuming this is a user on here, results look good! If anyone out there would be willing to perform this service for a fee I would be more than happy to send some cash their way!

Looks like this is the reverse LPF mod from buttersoft, see in [this thread](https://shmups.system11.org/viewtopic.php?p=1384663#p1384663) rather than the digital solution seen here. Looks good though!

---

## #119 — incrediblehark
*Sun Feb 26, 2023 1:03 am*


Ah, thank you for the correction!
