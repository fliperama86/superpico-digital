# Sharp analog RGB for the 3-Chip SNES using digital signals — Page 1

---

## Post by **Opatus**
*2020-05-26T11:55:04+00:00*


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

![Image](https://i.imgur.com/9wiTPiK.jpg?1)

Here is a video of Yoshi's Island with 1 frame of flicker at around 0:03: 

https://youtu.be/cSWN-uA_Ibc

And here are photos and videos of the project (unforunately I couldn't do a direct capture):

[https://www.dropbox.com/sh/5pgdqq6ppjmb ... ffgza?dl=0](https://www.dropbox.com/sh/5pgdqq6ppjmb4co/AAC5k4k9Di1v9dcw7J4bffgza?dl=0)

All photos are taken using the OSSC in Line5X mode on a 4k Samsung TV.

---

## Post by **yoshiyukiblade**
*2020-05-30T09:10:25+00:00*


This looks very promising! If all the remaining issues can be worked out, then we can finally have a perfect video solution. Best part is that it's the original non-1CHIP hardware.

I've been working on a pure analog solution to clean up the picture, but it will never look that good. Maybe I can finally put this project to rest.

---

## Post by **captaineos**
*2020-05-31T02:12:05+00:00*


I read through your posts on the DE forum and then found them here.  So great to bring awareness to your efforts as they appear to be a great idea and potential solution for us all.  

I have put a fair amount of effort into my 2-chip PPU PAL SNES but still get sad when I see the blurring of black lines.  Some games are worse than others.  E.g I don't really notice it on Super Mario Kart or Super Street Fighter but 240p text suit and Zelda are definitely too soft. 

My 80s computing understanding of digital RGB is that 8 colours can be pushed to 16 with intensity but that's the cap for TTL RGB.  When you say digital RGB but show a beautiful gamut on your TV, does this mean it's not TTL RGB but something else?  The closeups of SMW and Zelda are so crisp and colourful that I am amazed!

By all means keep us updated as I'm keen to keep my existing hardware and upgrade the RGB out with your ideas.

---

## Post by **Unseen**
*2020-05-31T10:26:41+00:00*


> 

captaineos wrote:When you say digital RGB but show a beautiful gamut on your TV, does this mean it's not TTL RGB but something else?

CGA (what you call TTL) is also digital RGB, but it uses just one bit per color channel and one additional bit for global brightness, so you can have two different values per color channel multiplied by two possible global brighenesses, giving 2*2*2*2=16 colors in total.

The digital RGB data on the SNES' test port is 5 bits per color, which results in 2*2*2*2*2=32 different shades for each color channel or 32*32*32=32768 colors total. There is an additional 4 bit wide brightness register in the PPU that can globally make the screen darker (e.g. for dafing the screen), but calculating how many additional colors can be created using it is a bit more complicated.

---

## Post by **Harrumph**
*2020-06-01T13:18:09+00:00*


This is great, seems to have overcome some hurdles that were not thought possible before!

> 

Opatus wrote: 

After some tests and slight CPLD code adjustments I have tested it in my working SNES unit as well. The brightness setting works and in Pilotwings the start screen looks like it should (uses the problematic Mode 7 settings).

So how did you actually solve the part with the brightness setting, you can tap if from somewhere else?

I checked out the original thread briefly, good to see the major players on the german scene all seems to have been involved (Ikari, Unseen, Borti etc).

---

## Post by **Opatus**
*2020-06-02T09:18:51+00:00*


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

## Post by **yoshiyukiblade**
*2020-06-02T21:00:50+00:00*


> 

Opatus wrote:
Was it the japanese solution or something different?

It's my own design, but largely inspired by the Japanese mod. I think I'll keep working on it until I'm mostly satisfied with the result. It's still "safe" in the sense that it's based on the PPU's analog output and nothing else, but of course it's not the best quality attainable. Anyway, I look forward to see where the digital mod goes!

---

## Post by **Opatus**
*2020-06-05T22:14:22+00:00*


I think I've found your channel on Youtube. The last video looked really nice with the mod.

I have changed the PPU2 and the flicker is gone. So it might have been just a soldering issue. 

The composite output looks the way it should as well, the blue color was missing before.

I had to revert back to csync for the digital output enable signal. The start screen of Super Mario World is broken again. It seems that it can be solved with a quick automatic reset after the system has powered up.

Beside a slight noise on some colors the output looks perfect with an OSSC. But it could be problematic with a CRT as there is a 50-100ns ringing at color changes. It comes from the way the brightness is mixed with the RGB signal. I create a voltage reference with a buffered R2R DAC and the digital signals coming from the PPU are switching between this brightness reference voltage and GND. These are then ran through the color R2R DACs. The problem is that at every digital signal edge the voltage reference gets pulled and starts ringing. 

If someone has an idea how the brightness could be combined with the RGB signal I am open for it.

![Image](https://i.imgur.com/LfvIRHz.jpg?1)

---

## Post by **rama**
*2020-06-07T14:05:07+00:00*


Awesome that there's a new development here. Congrats! 

The brightness problem might be solvable with a digital processor / scaler board.

I could attempt something with the digital inputs on gbscontrol. Just need to find time to do it.

---

## Post by **Opatus**
*2020-06-10T14:00:48+00:00*


Yeah I think I will go the digital route.The reference voltage has to change too quickly and thus it is not strong enough for the DAC. I've added a capacitor (1uF) which made the noise mostly disappear but it has also killed the rise/fall time of the reference voltage. As I have read even Nintendo had the same problem with the 1Chip SNES.

The rise time issue on top of the image in Yoshi's Island:

![Image](https://i.imgur.com/jTdqHNe.jpg?1)

So I wrote a quick test code for the smallest MAXII CPLD and it seems that 3 5X4 bit multipliers will fit (or worst case a LUT for the multiplication results) and the 3x9 bit results can go to a ADV7123 for conversion.

The current version already has the dejitter function and provision for the $2134-D4 region patch (so the output pin of the PPU does not have to be overdriven). I will also add the SuperCIC and IGR as most of the data for those are already accessed. And everything should fit under the top shielding.

---

## Post by **Voultar**
*2020-06-11T01:35:43+00:00*


> 

Opatus wrote:Yeah I think I will go the digital route.The reference voltage has to change too quickly and thus it is not strong enough for the DAC. I've added a capacitor (1uF) which made the noise mostly disappear but it has also killed the rise/fall time of the reference voltage. As I have read even Nintendo had the same problem with the 1Chip SNES.

You are correct. In the 1964 I determined that the current-steering DAC in the 1CHIP ASIC was not able to stabilize the swing for the video quickly enough. Slightly increasing the capacitance helps with the ghosting/visual noise, but at the expense of increasing the ready-state of the video lines, resulting in that slightly dark line that you see. 

This all looks fantastic, BTW. If there's anything that I may do to help you. Let me know!

---

## Post by **yoshiyukiblade**
*2020-06-12T23:04:58+00:00*


I'm thinking about abandoning my previous RLPF project now that this breakthrough is progressing very well. I did a long 7-hour test a few days ago with Zelda 3, but it's still not quite there yet: https://www.twitch.tv/videos/645520068. The digital mod is clearly the way of the future, but it seems pretty complicated with regard to getting the brightness values. I might start small with learning how to make a quality DAC from the digital RGB signals first, then move forward from there. I'm still a novice in electric circuits, especially relating to digital stuff, so this is all still very new and exciting to me!

---

## Post by **rama**
*2020-06-13T00:07:55+00:00*


yoshiyukiblade:

Your work on restoring the analog output is really good!

Please don't let it go to waste 

It is relevant to all kinds of analog signal issues, and it could help improve some other console!

---

## Post by **mikechi2**
*2020-06-13T03:38:43+00:00*


> 

Voultar wrote:

> 

Opatus wrote:Yeah I think I will go the digital route.The reference voltage has to change too quickly and thus it is not strong enough for the DAC. I've added a capacitor (1uF) which made the noise mostly disappear but it has also killed the rise/fall time of the reference voltage. As I have read even Nintendo had the same problem with the 1Chip SNES.

You are correct. In the 1964 I determined that the current-steering DAC in the 1CHIP ASIC was not able to stabilize the swing for the video quickly enough. Slightly increasing the capacitance helps with the ghosting/visual noise, but at the expense of increasing the ready-state of the video lines, resulting in that slightly dark line that you see. 

This all looks fantastic, BTW. If there's anything that I may do to help you. Let me know!

What does that cap do? Is it decoupling for some internal reference? Just throwing out a wild idea, any merit in building a circuit that actively buffers the voltage?

---

## Post by **mikechi2**
*2020-06-13T03:41:41+00:00*


This is some truly amazing stuff.

You probably already thought of this, but I might suggest using a Video DAC instead of a R2R ladder: [https://www.analog.com/media/en/technic ... DV7125.pdf](https://www.analog.com/media/en/technical-documentation/data-sheets/ADV7125.pdf)

This will probably get rid of all your artifacts and generate perfect video levels.

---

## Post by **yoshiyukiblade**
*2020-06-14T00:18:30+00:00*


@rama

Okay, maybe I'll just put it on the shelf instead of abandon it.  I'll revisit it whenever it becomes relevant again. Optimal sampling is a smoke screen for the underlying issues that are still there. I wanna see how it can look like with a perfect source!

@mikechi2

That may save a lot of headaches about DACs. Forgive my newness to this, but can it scale down from 8 bits to 5?

I started learning about weighted sum DACs and did some basic LTSpice simulations. One problem is that the glitches can be quite large. You can see similar glitches quite clearly on the 2chip internal DAC (8x oversampling on OSSC): https://i.imgur.com/cVsCPf4.png. The 1CHIP's DAC remains quite clean: https://i.imgur.com/LpKwrtY.png. I haven't taken a look at it on the scope, but any glitches are so small that you don't really see it even with 8x oversampling. Soon I'm gonna get my backup stock SNES motherboard, lift the relevant pins, then grab some real data to simulate with.

---

## Post by **Unseen**
*2020-06-14T00:34:41+00:00*


> 

yoshiyukiblade wrote:That may save a lot of headaches about DACs. Forgive my newness to this, but can it scale down from 8 bits to 5?

Connect the 5 SNES bits to the high-order bits of the DAC and either leave the three lowest bits grounded or connect the three highest-order bits of the SNES to the lowest-order bits of the DAC. The first method reduces the maximum brightness slightly which could be compensated by tweaking Rset, the second method reaches the full brightness, but introduces a slight nonlinearity.

For the SNES this isn't a problem though, after multiplying the 5 bit color value with the 4 bit content of the brightness register each color channel is 9 bits wide and that extra bit must be either dropped or processed into the other 8 to feed that DAC.

---

## Post by **yoshiyukiblade**
*2020-06-14T14:40:14+00:00*


So there are a number of ways to approach this. That's pretty neat.

I did some of the modifications posted in this thread and started poking around a bit. Lifted pins 37, 50 (tied both to VCC), and lifted 90-93. Pin 93 (TST15) was tied to the buffered CSYNC line. It seems to draw a fair bit of current because I originally tied it to pin 100, but the unbuffered CSYNC signal distorted pretty badly.

The analog video output looks mostly good, but I did notice some mode7-related glitches. From what I've read, glitches occur in both the analog and digital outputs. I'm not sure if this was a problem that was eventually solved by other tweaks.

Chrono Trigger pendulum: https://i.imgur.com/kZox8Y4.png When the pendulum swings far right, this graphic swings into view briefly

Super Metroid Ceres: https://i.imgur.com/eb716R2.png Multiple copies of Ridley fly out

Super Metroid planet Zebes arrival: https://i.imgur.com/OMOnSj2.png Multiple copes of the ship appear as it rotates counter clockwise

Anyway, I did start grabbing scope data and used it in my simulated DAC circuit with success.

---

## Post by **Opatus**
*2020-06-14T22:28:26+00:00*


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

## Post by **yoshiyukiblade**
*2020-06-15T00:06:56+00:00*


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

## Post by **yoshiyukiblade**
*2020-06-15T12:48:32+00:00*


I tested some combinations of OVER1/2 with VCC and the original OVER bus, but found nothing particularly useful. Leaving OVER2 floating seemed to function the same as pulling both high. Putting OVER2 back on the original OVER bus results in some additional minor glitches in the problem screens (in CT and Pilotwings). Other combinations resulted in glitches ranging from minor to severe.

As a sanity check. I put everything back to mostly stock form and the glitches went away. TST15 has to be pulled low or the glitches remain if left floating. I left TST12-14 floating, but that didn't seem to affect anything.

Out of curiosity, I tied TST15 to VCC and it seems to function the same as using the CSYNC signal (with OVER1/2 pulled high as well). I read some old discussion about using CSYNC to fix some issues that I haven't seen when OVER1/2 are pulled high. All of this this analysis is done on the analog output though. I'm not sure if there are significant differences in the digital output.

Do you think checking the brightness register can result in all the different problems you've seen?

---

## Post by **Unseen**
*2020-06-15T15:19:22+00:00*


> 

yoshiyukiblade wrote:Out of curiosity, I tied TST15 to VCC and it seems to function the same as using the CSYNC signal (with OVER1/2 pulled high as well). I read some old discussion about using CSYNC to fix some issues that I haven't seen when OVER1/2 are pulled high.

IIRC with TST15 pulled high permanently, Pilotwings has a broken palette and the pendulum in the intro of Chrono Trigger is missing while it is still in its swinging animation phase. I think this applied both to the digital TST output as well as the analog one.

---

## Post by **Opatus**
*2020-06-16T10:49:17+00:00*


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

## Post by **yoshiyukiblade**
*2020-06-16T15:25:59+00:00*


I don't know for certain how much current TST15 is drawing, but I'm not sure what else can cause the unbuffered CSYNC signal to distort.

Here are a couple clips of the Super Metroid problem:

https://youtu.be/8UojiuEssK8

https://youtu.be/ul37xbi9kS8

Other than these few issues I noticed, it has been working remarkably well. I haven't gotten any weird issues with color, sprite flicker, etc. I just have TST15, OVER1, OVER2 tied to VCC, and TST12-14 left floating.  I wonder if NTSC vs PAL have differences.

I'm slowly deciding on the parts to use for a DAC prototype. Since the digital pulses are used directly in the construction of the analog signal, I want to run all signals through high speed comparators to generate clean rectangles. I'm not sure if that's what you did on your board, but I think it's a good idea.

---

## Post by **Opatus**
*2020-06-16T15:50:27+00:00*


Thank you.

It seems like the Mode 7 is simply in screen repetition mode, strange. Not sure why do they look different, it is probably just because of the low resolution scaling.

I have just bought a copy at Ebay. Hopefully I recevie it until Friday.

If you touch the TST12-14 signals with your fingers you might get the color issues but maybe only when you power the system on, as TST15 is tied to VCC.

The rise time for the signals are decent so I use them directly with an analog switch to switch between the brightness voltage and GND. I don't think sharper rise times matter, certainly not for the OSSC and most likely not even for a BVM.

---

## Post by **Unseen**
*2020-06-16T20:31:50+00:00*


> 

yoshiyukiblade wrote:Since the digital pulses are used directly in the construction of the analog signal, I want to run all signals through high speed comparators to generate clean rectangles. I'm not sure if that's what you did on your board, but I think it's a good idea.

I think it's pointless to do that. If you use an integrated DAC, it will have an internal latch driven by the pixel clock that captures the digital value of the signals; if you want to build a discrete DAC, just add pixel-clock-driven latches yourself, e.g. using two 74HC573.

> 

I just have TST15, OVER1, OVER2 tied to VCC

I'm surprised there aren't any graphics issues with OVER1/2 tied high, although I can't point at any specific title where I would expect any.

---

## Post by **yoshiyukiblade**
*2020-06-17T01:14:25+00:00*


@Unseen

I want to build a discrete one mostly as an academic exercise. I didn't know that latches existed lol. I'll look more into this direction and make simulations where possible. Thank you for the suggestion! 

In the design I'm going for, the pulses need to be stable under some amount of resistive load for the summing amplifier. I don't want to use huge resistors, so devices that can source and sink a fair amount of current is ideal. The summing amp is non-inverting, so all 5 pulses need low impedance outputs for the weights to be accurate with respect to the resistor values.

@Opatus

Though rise times are not an issue, I'm concerned about noise patterns that are coupled in with the pulses. The OSSC might not show it with optimal sampling, but it might be visible at other sampling rates. For instance, the repetitive vertical bars at about 21 MHz are definitely visible at very high sample rates (2728 samples per scanline) even on a 1CHIP. Other patterns can be diagonals, scrolling or fixed. Probably not visible on a CRT for sure. Maybe I'm just paranoid for looking at this too long . I'm wondering if I can get rid of all that just by generating new pulses with a regulated low noise voltage source. It's a little over the top, but all in the name of curiosity!

---

## Post by **Opatus**
*2020-07-01T12:25:45+00:00*


I did not have much time to post here in the last 2 weeks.

In the meantime I've received Super Metroid and I had the same issues as well. I've also found a cheap PVM not far so I could check the mod on a crt. It looks great even with the broken DAC. Here's an image of it:

![Image](https://i.imgur.com/A4lj9Jq.jpg?1)

I've also tested Air Strike Patrol (Desert Fighter to be exact) to see if it is handled well. And as it was expected worked really well. The Hi-res mode showed no problem and the jetplane shadow (the mid-scanline brightness change) was also there. It was a bit faint due to the capactior on the reference voltage but it was visible.

For the mod itself as of right now I see 3 possbile solutions:

1. The already mentioned switching between the original and digital RGB, just like the 32X does. Of course it would not really be suited for an HMDI mod, also the quality difference can be a bit jarring. And third the difference in voltage for the 2 RGB signals. That means that after the digital RGB comes out of the DAC it has to be clamped otherwise there will be a difference in black levels. It is of course not a big problem.

I have already tried this looking at the composite output. It works, although I have seen a ship duplicate in Super Metroid during the intro. I think it happens because the PPU pipeline is a few pixels deep and I use OVER from PPU1 to switch between the signals. So this OVER has to be delayed and the duplicate should in theory disappear.

2. Change the PPU to the mode where it outputs character #0 when the screen wraps around. The only problem is that it is not guaranteed that character #0 is transparent. Most likely it is but as the Chrono Trigger image from yoshiyukiblade shows that the VRAM is already preloaded with data (not character #0 though). Most likely no game has any color at the first character but I don't know it.

3. If I'm correct this could be the real solution. I don't know how exactly the Mode7 is communicated between the 2 PPUs I'm just guessing, so please correct me or confirm it. I think the PPU1 calculates the VRAM coordinates for every pixel and then reads it from VRAM. PPU2 also receives this pixel as it listens to the bus and that's how it knows what color to use. So if that is correct all we have to do is when OVER from PPU1 is low set the VRAM data that the PPU2 receives to 0 and pull the OVER for the PPU2 high. That way the PPU2 will think the screen has not wrapped around yet  and will output color 0 (which it should normally do anyway). If this proves correct then this would be an en easy way the correct the problem.

I've also seen an issue during the Donkey Kong Country level "Millstone Mayhem". Half of the screen went green for a frame every 3-4 seconds (always at the same section of the level, probably Rare is doing something unorthodox once again). I was switching TST15 off during HBLANK, VBLANK and forced blanking. After that I've tied TST15 to Vcc -> the problem went away but the color palette got broken again. I will look into the exact timing and how to avoid it later.

![Image](https://i.imgur.com/RT37z9O.jpg?1)

---

## Post by **Unseen**
*2020-07-01T15:05:37+00:00*


> 

Opatus wrote:I think the PPU1 calculates the VRAM coordinates for every pixel and then reads it from VRAM. PPU2 also receives this pixel as it listens to the bus and that's how it knows what color to use.

My current understanding is that PPU1 gathers all potential data sources for the current pixel and sends them to PPU2, which applies palettes, colormath and at least some priorities(*) to calculate what the final pixel should look like.

(*) can't remember off-hand if PPU1 sorts out the priority between tile layers itself or not

> 

That way the PPU2 will think the screen has not wrapped around yet  and will output color 0 (which it should normally do anyway). If this proves correct then this would be an en easy way the correct the problem.

I don't think this is always correct, the game can select between three different wrapping methods (wrap within 128x128 tile map, use transparency, use tile 0). According to my old notes, /OVER from PPU1 is the signal that indicates if the current pixel is in the wrapped part of the mode7 area. IIRC the title screen of On The Ball uses wrapmode 2 (transparent).

---

## Post by **Opatus**
*2020-07-01T15:21:13+00:00*


> 

I don't think this is always correct, the game can select between three different wrapping methods (wrap within 128x128 tile map, use transparency, use tile 0). According to my old notes, /OVER from PPU1 is the signal that indicates if the current pixel is in the wrapped part of the mode7 area. IIRC the title screen of On The Ball uses wrapmode 2 (transparent).

Sorry I've meant only for this wrap mode. So do that if transparency should be used otherwise use the data given by PPU1.

I thought those pins are used for the sprites. They would not be enough for 256 or direct color backgrounds and 2 bits of priority. The PPU2 has to do the window effects so I assume it should know everything about the sprites and the backgrounds. It also does mosaic (according to the manual) and it is only performed on the backgrounds.

I will poke around a little bit if I find the time.
