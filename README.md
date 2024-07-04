# R5R_AimAssist_Forcer
Allows to set custom AA strength on controller aim assist in R5Reloaded. Only supports season 3 build of apex!
Check out my blog post related to it: https://drof.one/r5reloaded-custom-aim-assist-strength/

# Setup Video
https://www.youtube.com/watch?v=9pVbLUoPn-k

# How it works
In earlier seasons like the one used in R5Reloaded (Season 3), apex stored your aim assist strength in a global float:
```
    flAaStrength = getplaylistvar(v2, qword_1666EDE98, "aim_assist_magnet_pc", 1); 
    if ( flAaStrength )
    {
      sub_1411B2F0C(flAaStrength);
      v7 = v1;
      v8 = fminf(fmaxf(v7, 0.0), Number);
      if ( *(float *)&gflAimAssistValueMagnet != v8 ) // <------ aim assist uses gflAimAssistValueMagnet to determine strength
      {
        v2 = qword_16705B9F0;
        gflAimAssistValueMagnet = LODWORD(v8);
      }
      // ...
    }
```
This allows us to effortlessly alter our aim assist strength even in multiplayer, by overwriting `gflAimAssistValueMagnet` with your desired value. It has following format:

```
strength=20%  gflAimAssistValueMagnet_Value=0.2
strength=40%  gflAimAssistValueMagnet_Value=0.4
strength=60%  gflAimAssistValueMagnet_Value=0.6
strength=80%  gflAimAssistValueMagnet_Value=0.8
strength=100% gflAimAssistValueMagnet_Value=1.0
```
**Formula:** <desired_strength_value_percent> / 100<br/>
You don't have to rely on 20% steps, can set any value in between like `0.25`, `0.57`, etc.

# Risks
At the time of writing this, R5Reloaded does not have any anti-cheat protection, and will likely never adopt one. Additionally, this does not change any playlist vars which could be detected by server requesting a copy of your local playlist vars, for example. It only changes a global floating point variable, which controls the magnet strength of apex's controller aim assist. R5R_AAF is fully external, and only uses RPM/WPM APIs. Always use it at your own risk though, you are responsible for your account!

# Setup
1. open r5rsucks.sln and build it in x64 Release.
2. start R5Reloaded.
3. start r5rsucks.exe (found in x64/Release from source code folder)
4. set your desired aim assist value by following on screen instructions in cmd.

**NOTE:** I am already applying above formula for converting an user input percentage range (0-100%) to their corresponding `gflAimAssistValueMagnet_Value` value. You only need to give a percentage from 0 - 100%, R5R_AAF will do the rest. Additionally, you can see what your current aim assist strength is, by checking R5R_AAF window title.

**NOTE #02:** You will also have to re-apply your forced aim assist value after disconnecting/reconnecting from/to a game server.


# Conclusion
So this was a pretty basic project, but i still had fun researching how apex legends' controller aim assist works internally. I know that some game server hosters in R5Reloaded attempt to lower aim assist values to like 20%, which is one reason why i wanted to release it. It's also so different from how retail apex does their aim assist stuff, that this will not work on season 21 retail apex legends. So it's no threat to the retail version of apex. It would get picked up by their anti cheat anyways.
