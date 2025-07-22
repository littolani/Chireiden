#include "AnmVm.h"

void AnmVm::initialize()
{
    D3DXVECTOR3 entityPos = this->entityPos;
    int layer = this->layer;
    memset(this,0,0x434);

    this->scale.x = 1.0;
    this->scale.y = 1.0;
    this->entityPos = entityPos;
    this->layer = layer;
    this->color1 = 0xffffffff;
    D3DXMatrixIdentity(&this->matrix2fc);
    this->flagsLow = 7;
    this->timeInScript.m_current = 0;
    this->timeInScript.m_currentF = 0.0;
    this->timeInScript.m_previous = -999999;
    this->posInterp.endTime = 0;
    this->rgbInterp.endTime = 0;
    this->alphaInterp.endTime = 0;
    this->rotationInterp.endTime = 0;
    this->scaleInterp.endTime = 0;
    this->rgb2Interp.endTime = 0;
    this->alpha2Interp.endTime = 0;
    this->uVelInterp.endTime = 0;
    this->vVelInterp.endTime = 0;
    this->nodeInGlobalList.next = nullptr;
    this->nodeInGlobalList.prev = nullptr;
    this->nodeInGlobalList.entry = this;
    this->nodeAsFamilyMember.next = nullptr;
    this->nodeAsFamilyMember.prev = nullptr;
    this->nodeAsFamilyMember.entry = this;
}

void AnmVm::run() {
    while (currentInstruction != nullptr)
    {
        if (flagsLow & 0x20000)
            return; // Early exit based on flag

        if (pendingInterrupt != 0)
        {
            // Interrupt handling: search for opcode 64 with matching label
            AnmRawInstruction* instr = beginningOfScript;
            while (instr->opcode != 0xFFFF)
            {
                if (instr->opcode == 64 && instr->args[0] == pendingInterrupt) {
                    interruptReturnInstr = currentInstruction;
                    interruptReturnTime = timeInScript;
                    timeInScript.setCurrent(instr->time);
                    currentInstruction = instr;
                    pendingInterrupt = 0;
                    flagsLow |= 1; // Set visibility
                    break;
                }
                instr = (AnmRawInstruction*)((char*)instr + instr->offsetToNextInstr);
            }
        } 
        
        else if (currentInstruction->time <= timeInScript.m_current)
        {
            AnmRawInstruction* instr = currentInstruction;
            uint16_t opcode = instr->opcode;
            switch (opcode)
            {
                // Does nothing.
                case 0: // nop
                    break;

                // Destroys the graphic.
                // EoSD: this instruction and static may only appear as the final instruction of a script, and one of these must appear. thstd and trustd will implicitly insert a delete at the closing brace if there isn't one.
                // PCB–: this instruction may appear anywhere. Additionally, a VM that reaches the end of the script will be automatically deleted as if it encountered this instruction.
                case 1: // delete
                    flagsLow &= ~1; // Clear visibility
                    currentInstruction = nullptr; // Terminate script
                    break;

                // Freezes the graphic until it is destroyed externally.
                // Any interpolation instructions like posTime will no longer advance, and interrupts are disabled.
                // In EoSD only, this instruction and delete may only appear as the final instruction of a script.
                case 2: // static
                    flagsLow |= 0x10000; // Set static flag (placeholder)
                    break;

                // Sets the image used by this VM to one of the sprites defined in the ANM file. A value of -1 means to not use an image (this is frequently used with special drawing instructions). thanm also lets you use the sprite's name instead of an index.
                // Under some unknown conditions, these sprite indices are transformed by a "sprite-mapping" function; e.g. many bullet scripts use false indices, presumably to avoid repeating the same script for 16 different colors. The precise mechanism of this is not yet fully understood.
                case 3: // sprite(int id)
                    spriteId = getIntArg(this, instr, 0); // Set sprite ID
                    break;

                // Jumps to byte offset dest from the script's beginning and sets the time to t. thanm accepts a label name for dest.
                // Chinese wiki says some confusing recommendation about setting a=0, can someone explain to me?
                case 4: // jmp(int dest, int t)
                    currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 0));
                    timeInScript.setCurrent(getIntArg(this, instr, 1));
                    break;

                // Decrement x and then jump if x > 0. You can use this to repeat a loop a fixed number of times.
                case 5: // jmpDec(int& x, int dest, int t)
                {
                    int* x = getIntVarPtr(this, instr->args[0]);
                    if (*x > 0)
                    {
                        *x -= 1;
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 1));
                        timeInScript.setCurrent(getIntArg(this, instr, 2));
                    }
                }
                break;

                // Does a = b.
                case 6: // iset(int& a, int b)
                    *getIntVarPtr(this, instr->args[0]) = getIntArg(this, instr, 1);
                    break;

                // Does a = b.
                case 7: // fset(float& a, float b)
                    *getFloatVarPtr(this, instr->args[0]) = getFloatArg(this, instr, 1);
                    break;

                // Does a += b.
                case 8: // iadd(int& a, int b)
                    *getIntVarPtr(this, instr->args[0]) += getIntArg(this, instr, 1);
                    break;

                // Does a += b.
                case 9: // fadd(float& a, float b)
                    *getFloatVarPtr(this, instr->args[0]) += getFloatArg(this, instr, 1);
                    break;

                // Does a -= b.
                case 10: // isub(int& a, int b)
                    *getIntVarPtr(this, instr->args[0]) -= getIntArg(this, instr, 1);
                    break;

                // Does a -= b.
                case 11: // fsub(float& a, float b)
                    *getFloatVarPtr(this, instr->args[0]) -= getFloatArg(this, instr, 1);
                    break;

                // Does a *= b.
                case 12: // imul(int& a, int b)
                    *getIntVarPtr(this, instr->args[0]) *= getIntArg(this, instr, 1);
                    break;

                // Does a *= b.
                case 13: // fmul(float& a, float b)
                    *getFloatVarPtr(this, instr->args[0]) *= getFloatArg(this, instr, 1);
                    break;

                // Does a /= b.
                case 14: // idiv(int& a, int b)
                    {
                        int b = getIntArg(this, instr, 1);
                        if (b != 0) *getIntVarPtr(this, instr->args[0]) /= b;
                    }
                    break;

                // Does a /= b.
                case 15: // fdiv(float& a, float b)
                    {
                        float b = getFloatArg(this, instr, 1);
                        if (b != 0.0f) *getFloatVarPtr(this, instr->args[0]) /= b;
                    }
                    break;

                // Does a %= b.
                case 16: // imod(int& a, int b)
                    {
                        int b = getIntArg(this, instr, 1);
                        if (b != 0) *getIntVarPtr(this, instr->args[0]) %= b;
                    }
                    break;

                // Does a %= b.
                case 17: // fmod(float& a, float b)
                    {
                        float b = getFloatArg(this, instr, 1);
                        if (b != 0.0f) *getFloatVarPtr(this, instr->args[0]) = fmodf(*getFloatVarPtr(this, instr->args[0]), b);
                    }
                    break;

                // Does a = b + c.
                case 18: // isetAdd(int& x, int a, int b)
                    *getIntVarPtr(this, instr->args[0]) = getIntArg(this, instr, 1) + getIntArg(this, instr, 2);
                    break;

                // Does a = b + c.
                case 19: // fsetAdd(float& x, float a, float b)
                    *getFloatVarPtr(this, instr->args[0]) = getFloatArg(this, instr, 1) + getFloatArg(this, instr, 2);
                    break;

                // Does a = b - c.
                case 20: // isetSub(int& x, int a, int b)
                    *getIntVarPtr(this, instr->args[0]) = getIntArg(this, instr, 1) - getIntArg(this, instr, 2);
                    break;

                // Does a = b - c.
                case 21: // fsetSub(float& x, float a, float b)
                    *getFloatVarPtr(this, instr->args[0]) = getFloatArg(this, instr, 1) - getFloatArg(this, instr, 2);
                    break;

                // Does a = b * c.
                case 22: // isetMul(int& x, int a, int b)
                    *getIntVarPtr(this, instr->args[0]) = getIntArg(this, instr, 1) * getIntArg(this, instr, 2);
                    break;

                // Does a = b * c.
                case 23: // fsetMul(float& x, float a, float b)
                    *getFloatVarPtr(this, instr->args[0]) = getFloatArg(this, instr, 1) * getFloatArg(this, instr, 2);
                    break;

                // Does a = b / c.
                case 24: // isetDiv(int& x, int a, int b)
                    {
                        int b = getIntArg(this, instr, 2);
                        if (b != 0) *getIntVarPtr(this, instr->args[0]) = getIntArg(this, instr, 1) / b;
                    }
                    break;

                // Does a = b / c.
                case 25: // fsetDiv(float& x, float a, float b)
                    {
                        float b = getFloatArg(this, instr, 2);
                        if (b != 0.0f) *getFloatVarPtr(this, instr->args[0]) = getFloatArg(this, instr, 1) / b;
                    }
                    break;

                // Does a = b % c.
                case 26: // isetMod(int& x, int a, int b)
                    {
                        int b = getIntArg(this, instr, 2);
                        if (b != 0) *getIntVarPtr(this, instr->args[0]) = getIntArg(this, instr, 1) % b;
                    }
                    break;

                // Does a = b % c.
                case 27: // fsetMod(float& x, float a, float b)
                    {
                        float b = getFloatArg(this, instr, 2);
                        if (b != 0.0f) *getFloatVarPtr(this, instr->args[0]) = fmodf(getFloatArg(this, instr, 1), b);
                    }
                    break;

                // Jumps if a == b.
                case 28: // ije(int a, int b, int dest, int t)
                    if (getIntArg(this, instr, 0) == getIntArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a == b.
                case 29: // fje(float a, float b, int dest, int t)
                    if (getFloatArg(this, instr, 0) == getFloatArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a != b.
                case 30: // ijne(int a, int b, int dest, int t)
                    if (getIntArg(this, instr, 0) != getIntArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a != b.
                case 31: // fjne(float a, float b, int dest, int t)
                    if (getFloatArg(this, instr, 0) != getFloatArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a < b.
                case 32: // ijl(int a, int b, int dest, int t)
                    if (getIntArg(this, instr, 0) < getIntArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a < b.
                case 33: // fjl(float a, float b, int dest, int t)
                    if (getFloatArg(this, instr, 0) < getFloatArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a <= b.
                case 34: // ijle(int a, int b, int dest, int t)
                    if (getIntArg(this, instr, 0) <= getIntArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a <= b.
                case 35: // fjle(float a, float b, int dest, int t)
                    if (getFloatArg(this, instr, 0) <= getFloatArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a > b.
                case 36: // ijg(int a, int b, int dest, int t)
                    if (getIntArg(this, instr, 0) > getIntArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a > b.
                case 37: // fjg(float a, float b, int dest, int t)
                    if (getFloatArg(this, instr, 0) > getFloatArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a >= b.
                case 38: // ijge(int a, int b, int dest, int t)
                    if (getIntArg(this, instr, 0) >= getIntArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Jumps if a >= b.
                case 39: // fjge(float a, float b, int dest, int t)
                    if (getFloatArg(this, instr, 0) >= getFloatArg(this, instr, 1)) {
                        currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + getIntArg(this, instr, 2));
                        timeInScript.setCurrent(getIntArg(this, instr, 3));
                    }
                    break;

                // Draw a random integer 0 <= x < n using the animation RNG.
                case 40: // isetRand(int& x, int n)
                    *getIntVarPtr(this, instr->args[0]) = randInt(0, getIntArg(this, instr, 1)); // Placeholder randInt
                    break;

                // Draw a random float 0 <= x <= r using the animation RNG.
                case 41: // fsetRand(float& x, float r)
                    *getFloatVarPtr(this, instr->args[0]) = randFloat(0.0f, getFloatArg(this, instr, 1)); // Placeholder randFloat
                    break;

                // Compute sin(θ) (θ in radians).
                case 42: // fsin(float& dest, float θ)
                    *getFloatVarPtr(this, instr->args[0]) = sinf(getFloatArg(this, instr, 1));
                    break;

                // Compute cos(θ) (θ in radians).
                case 43: // fcos(float& dest, float θ)
                    *getFloatVarPtr(this, instr->args[0]) = cosf(getFloatArg(this, instr, 1));
                    break;

                // Compute tan(θ) (θ in radians).
                case 44: // ftan(float& dest, float θ)
                    *getFloatVarPtr(this, instr->args[0]) = tanf(getFloatArg(this, instr, 1));
                    break;

                // Compute acos(x) (output in radians).
                case 45: // facos(float& dest, float x)
                    *getFloatVarPtr(this, instr->args[0]) = acosf(getFloatArg(this, instr, 1));
                    break;

                // Compute atan(m) (output in radians).
                case 46: // fatan(float& dest, float m)
                    *getFloatVarPtr(this, instr->args[0]) = atanf(getFloatArg(this, instr, 1));
                    break;

                // Reduce an angle modulo 2*PI into the range [-PI, +PI].
                case 47: // validRad(float& θ)
                    {
                        float* theta = getFloatVarPtr(this, instr->args[0]);
                        *theta = fmodf(*theta + M_PI, 2.0f * M_PI) - M_PI;
                    }
                    break;

                // Sets the position of the graphic.
                // If you look in the code, you'll see that if a certain bitflag is set, this will write to a different variable. This is part of the implementation of th09:posMode in earlier games, and is, to my knowledge, entirely dead code in every game since StB.
                case 48: // pos(float x, float y, float z)
                    pos.x = getFloatArg(this, instr, 0);
                    pos.y = getFloatArg(this, instr, 1);
                    pos.z = getFloatArg(this, instr, 2);
                    break;

                // Set the graphic's rotation. For 2D objects, only the z rotation matters.
                // In some rare cases, x rotation has a special meaning for special drawing instructions. Graphics rotate around their anchor point (see anchor).
                // A positive angle around the z-axis goes clockwise from the +x direction towards the +y direction (defined to point down). 3D rotations are performed as follows:
                // (EoSD) Rotate first around x, then around y, then around z.
                // (PCB–GFW) Haven't checked. Probably the same?
                // (TD–) You can choose the rotation system with th185:rotationMode. (what's the default? probably the same?)
                // If nothing seems to be happening when you call this, check your type setting!
                case 49: // rotate(float rx, float ry, float rz)
                    rotation.x = getFloatArg(this, instr, 0);
                    rotation.y = getFloatArg(this, instr, 1);
                    rotation.z = getFloatArg(this, instr, 2);
                    break;

                // Scales the ANM independently along the x and y axis. Graphics grow around their anchor point (see anchor). Some special drawing instructions give the x and y scales special meaning.
                case 50: // scale(float sx, float sy)
                    scale.x = getFloatArg(this, instr, 0);
                    scale.y = getFloatArg(this, instr, 1);
                    break;

                // Set alpha (opacity) to a value 0-255.
                case 51: // alpha(int alpha)
                    alpha = getIntArg(this, instr, 0);
                    break;

                // Set a color which gets blended with this sprite. Blend operation is multiply, so setting white (255, 255, 255) eliminates the effect.
                case 52: // color(int r, int g, int b)
                    color.r = getIntArg(this, instr, 0);
                    color.g = getIntArg(this, instr, 1);
                    color.b = getIntArg(this, instr, 2);
                    break;

                // Set a constant angular velocity, in radians per frame.
                case 53: // angleVel(float ωx, float ωy, float ωz)
                    angularVelocity.x = getFloatArg(this, instr, 0);
                    angularVelocity.y = getFloatArg(this, instr, 1);
                    angularVelocity.z = getFloatArg(this, instr, 2);
                    break;

                // Every frame, it increases the values of scale as sx -> sx + gx and sy -> sy + gy. Basically, scaleGrowth is to scale as angleVel is to rotate. (they even share implemenation details...)
                case 54: // scaleGrowth(float gx, float gy)
                    scaleGrowth.x = getFloatArg(this, instr, 0);
                    scaleGrowth.y = getFloatArg(this, instr, 1);
                    break;

                // Obsolete. Use alphaTime instead.
                // UNTESTED: Linearly changes alpha to alpha over the next t frames. Identical to calling alphaTime(t, 0, alpha).
                case 55: // alphaTimeLinear(int alpha, int t)
                    alphaInterp.m_endTime = getIntArg(this, instr, 1);
                    alphaInterp.m_method = 0; // Linear
                    alphaInterp.m_initial = alpha;
                    alphaInterp.m_goal = getIntArg(this, instr, 0);
                    alphaInterp.m_timer.setCurrent(0);
                    break;

                // Over the next t frames, changes pos to the given values using interpolation mode mode.
                case 56: // posTime(int t, int mode, float x, float y, float z)
                    posInterp.m_endTime = getIntArg(this, instr, 0);
                    posInterp.m_method = getIntArg(this, instr, 1);
                    posInterp.m_goal.x = getFloatArg(this, instr, 2);
                    posInterp.m_goal.y = getFloatArg(this, instr, 3);
                    posInterp.m_goal.z = getFloatArg(this, instr, 4);
                    posInterp.m_initial = pos;
                    posInterp.m_timer.setCurrent(0);
                    break;

                // Over the next t frames, changes color to the given value using interpolation mode mode.
                case 57: // colorTime(int t, int mode, int r, int g, int b)
                    colorInterp.m_endTime = getIntArg(this, instr, 0);
                    colorInterp.m_method = getIntArg(this, instr, 1);
                    colorInterp.m_goal.r = getIntArg(this, instr, 2);
                    colorInterp.m_goal.g = getIntArg(this, instr, 3);
                    colorInterp.m_goal.b = getIntArg(this, instr, 4);
                    colorInterp.m_initial = color;
                    colorInterp.m_timer.setCurrent(0);
                    break;

                // Over the next t frames, changes alpha to the given values using interpolation mode mode.
                case 58: // alphaTime(int t, int mode, int alpha)
                    alphaInterp.m_endTime = getIntArg(this, instr, 0);
                    alphaInterp.m_method = getIntArg(this, instr, 1);
                    alphaInterp.m_goal = getIntArg(this, instr, 2);
                    alphaInterp.m_initial = alpha;
                    alphaInterp.m_timer.setCurrent(0);
                    break;

                // Over the next t frames, changes rotate to the given values using interpolation mode mode.
                case 59: // rotateTime(int t, int mode, float rx, float ry, float rz)
                    rotationInterp.m_endTime = getIntArg(this, instr, 0);
                    rotationInterp.m_method = getIntArg(this, instr, 1);
                    rotationInterp.m_goal.x = getFloatArg(this, instr, 2);
                    rotationInterp.m_goal.y = getFloatArg(this, instr, 3);
                    rotationInterp.m_goal.z = getFloatArg(this, instr, 4);
                    rotationInterp.m_initial = rotation;
                    rotationInterp.m_timer.setCurrent(0);
                    break;

                // Over the next t frames, changes scale to the given values using interpolation mode mode.
                case 60: // scaleTime(int t, int mode, float sx, float sy)
                    scaleInterp.m_endTime = getIntArg(this, instr, 0);
                    scaleInterp.m_method = getIntArg(this, instr, 1);
                    scaleInterp.m_goal.x = getFloatArg(this, instr, 2);
                    scaleInterp.m_goal.y = getFloatArg(this, instr, 3);
                    scaleInterp.m_initial = scale;
                    scaleInterp.m_timer.setCurrent(0);
                    break;

                // Toggles mirroring on the x axis.
                // This flips the sign of sx in scale. Future calls to scale will thus clobber it.
                // (It also toggles a bitflag. This flag is used by EoSD to keep the sign flipped during interpolation of scale, and is then unused until BM which added th185:unflip.)
                case 61: // flipX
                    scale.x = -scale.x;
                    flagsLow ^= 0x2; // Toggle flipX flag
                    break;

                // Toggles mirroring on the y axis.
                // This flips the sign of sy in scale. Future calls to scale will thus clobber it.
                // (It also toggles a bitflag. This flag is used by EoSD to keep the sign flipped during interpolation of scale, and is then unused until BM which added th185:unflip.)
                case 62: // flipY
                    scale.y = -scale.y;
                    flagsLow ^= 0x4; // Toggle flipY flag
                    break;

                // Stops executing the script (at least for now), but keeps the graphic alive.
                // Interpolation instructions like posTime will continue to advance, and interrupts can be triggered at any time. You could say this essentially behaves like an infinitely long wait.
                case 63: // stop
                    flagsLow |= 0x8000; // Set stopped flag (placeholder)
                    break;

                // A label for an interrupt. When executed, it is a no-op.
                case 64: // interruptLabel(int n)
                    // No-op
                    break;

                // Set the horizontal and vertical anchoring of the sprite. Notice the args are each two bytes. For further fine-tuning see th185:anchorOffset.
                // Args: 0=Center, 1=Left, 2=Right (h); 0=Center, 1=Top, 2=Bottom (v)
                case 65: // anchor(short h, short v)
                    anchorH = getIntArg(this, instr, 0);
                    anchorV = getIntArg(this, instr, 1);
                    break;

                // Set color blending mode.
                // Modes for DDC: (other games may be different)
                // 0: Normal (SRCALPHA, INVSRCALPHA, ADD)
                // 1: Add (SRCALPHA, ONE, ADD)
                // 2: Subtract (SRCALPHA, ONE, REVSUBTRACT)
                // ...
                case 66: // blendMode(int mode)
                    blendMode = getIntArg(this, instr, 0);
                    break;

                // Determines how the ANM is rendered.
                // Mode 0: 2D sprites, no rotation.
                // Mode 1: 2D sprites with z-axis rotation.
                // Mode 8: 3D rotation.
                // Mode 2: Like mode 0 but shifted by (-0.5, -0.5) pixels.
                case 67: // type(int mode)
                    renderMode = getIntArg(this, instr, 0);
                    break;

                // Sets the layer of the ANM. This may or may not affect z-ordering? It's weird...
                // Different layer numbers may behave differently! Each game only has a finite number of layers, and certain groups of these layers are drawn at different stages in the rendering pipeline.
                case 68: // layer(int n)
                    layer = getIntArg(this, instr, 0);
                    break;

                // This is like stop except that it also hides the graphic by clearing the visibility flag (see visible).
                // Interpolation instructions like posTime will continue to advance, and interrupts can be triggered at any time. Successful interrupts will automatically re-enable the visibility flag.
                case 69: // stopHide
                    flagsLow |= 0x8000; // Set stopped flag
                    visible = 0;
                    break;

                // Add vel to the texture u coordinate every frame (in units of 1 / total_image_width), causing the displayed sprite to scroll horizontally through the image file.
                case 70: // scrollX(float vel)
                    scrollU = getFloatArg(this, instr, 0);
                    break;

                // Add vel to the texture v coordinate every frame (in units of 1 / total_image_height), causing the displayed sprite to scroll vertically through the image file.
                case 71: // scrollY(float vel)
                    scrollV = getFloatArg(this, instr, 0);
                    break;

                // Set the visibility flag (1 = visible). Invisible graphics are skipped during rendering.
                // Generally speaking this is not a huge deal as the most expensive parts of rendering are typically skipped anyways whenever alpha and alpha2 are both 0.
                case 72: // visible(byte visible)
                    visible = getIntArg(this, instr, 0);
                    break;

                // If disable is 1, writing to the z-buffer is disabled. Otherwise, it is enabled. This can matter in cases where z-testing is used to filter writes.
                case 73: // zWriteDisable(int disable)
                    zWriteDisable = getIntArg(this, instr, 0);
                    break;

                // Sets the state of a STD-related bitflag. When this flag is enabled, some unknown vector from the stage background camera data is added to some poorly-understood position-related vector on the ANM, for an even more unknown purpose.
                case 74: // ins_74(int enable)
                    // TODO: Implement STD-related bitflag logic
                    break;

                // Wait t frames.
                // StB and earlier: wait is implemented using a dedicated timer.
                // MoF and later: Subtracts t from the current time before executing the next instruction.
                case 75: // wait(int t)
                    timeInScript.setCurrent(timeInScript.m_current - getIntArg(this, instr, 0)); // MoF+ behavior
                    break;

                // Set a second color for gradients. Gradients are used by certain special drawing instructions, and can be enabled on regular sprites using colorMode.
                case 76: // color2(int r, int g, int b)
                    color2.r = getIntArg(this, instr, 0);
                    color2.g = getIntArg(this, instr, 1);
                    color2.b = getIntArg(this, instr, 2);
                    break;

                // Set a second alpha for gradients. Gradients are used by certain special drawing instructions, and can be enabled on regular sprites using colorMode.
                case 77: // alpha2(int alpha)
                    alpha2 = getIntArg(this, instr, 0);
                    break;

                // Over the next t frames, changes color2 to the given value using interpolation mode mode.
                // For some reason, in DDC onwards, this also sets colorMode to 1, which can be a mild inconvenience.
                case 78: // color2Time(int t, int mode, int r, int g, int b)
                    color2Interp.m_endTime = getIntArg(this, instr, 0);
                    color2Interp.m_method = getIntArg(this, instr, 1);
                    color2Interp.m_goal.r = getIntArg(this, instr, 2);
                    color2Interp.m_goal.g = getIntArg(this, instr, 3);
                    color2Interp.m_goal.b = getIntArg(this, instr, 4);
                    color2Interp.m_initial = color2;
                    color2Interp.m_timer.setCurrent(0);
                    colorMode = 1; // DDC+ behavior
                    break;

                // Over the next t frames, changes alpha2 to the given value using interpolation mode mode.
                // For some reason, in DDC onwards, this also sets colorMode to 1, which can be a mild inconvenience.
                case 79: // alpha2Time(int t, int mode, int alpha)
                    alpha2Interp.m_endTime = getIntArg(this, instr, 0);
                    alpha2Interp.m_method = getIntArg(this, instr, 1);
                    alpha2Interp.m_goal = getIntArg(this, instr, 2);
                    alpha2Interp.m_initial = alpha2;
                    alpha2Interp.m_timer.setCurrent(0);
                    colorMode = 1; // DDC+ behavior
                    break;

                // Lets you enable gradients on regular sprites.
                // 0: Only use color and alpha.
                // 1: Only use color2 and alpha2.
                // 2: (DS–) Horizontal gradient.
                // 3: (DS–) Vertical gradient.
                case 80: // colorMode(int mode)
                    colorMode = getIntArg(this, instr, 0);
                    break;

                // Can be used at the end of a interruptLabel block to return back to the moment just before the VM received the interrupt.
                // This is not the only way to end a interruptLabel block; oftentimes the game may use a stop instead.
                case 81: // caseReturn
                    currentInstruction = interruptReturnInstr;
                    timeInScript = interruptReturnTime;
                    break;

                // Placeholder for rotateAuto(byte mode)
                case 82: // rotateAuto
                    // TODO: Implement rotateAuto logic
                    break;

                // Placeholder for ins_83()
                case 83: // ins_83
                    // TODO: Unknown instruction
                    break;

                // Placeholder for texCircle(int nmax)
                case 84: // texCircle
                    // TODO: Implement texCircle logic
                    break;

                // Placeholder for ins_85(int enable)
                case 85: // ins_85
                    // TODO: Unknown instruction
                    break;

                // Placeholder for slowdownImmune(int enable)
                case 86: // slowdownImmune
                    // TODO: Implement slowdownImmune logic
                    break;

                // Placeholder for randMode(int mode)
                case 87: // randMode
                    // TODO: Implement randMode logic
                    break;

                // Placeholder for scriptNew(int script)
                case 88: // scriptNew
                    // TODO: Implement script creation (calls 0x455a00)
                    break;

                // Placeholder for resampleMode(int n)
                case 89: // resampleMode
                    // TODO: Implement resampleMode logic
                    break;

                // Placeholder for scriptNewUI(int script)
                case 90: // scriptNewUI
                    // TODO: Implement scriptNewUI logic
                    break;

                // Placeholder for scriptNewFront(int script)
                case 91: // scriptNewFront
                    // TODO: Implement scriptNewFront logic
                    break;

                // Placeholder for scriptNewUIFront(int script)
                case 92: // scriptNewUIFront
                    // TODO: Implement scriptNewUIFront logic
                    break;

                // Placeholder for scrollXTime(int t, int mode, float vel)
                case 93: // scrollXTime
                    // TODO: Implement scrollXTime logic
                    break;

                // Placeholder for scrollYTime(int t, int mode, float vel)
                case 94: // scrollYTime
                    // TODO: Implement scrollYTime logic
                    break;

                // Placeholder for scriptNewRoot(int script)
                case 95: // scriptNewRoot
                    // TODO: Implement scriptNewRoot logic
                    break;

                // Placeholder for scriptNewPos(int script, float x, float y)
                case 96: // scriptNewPos
                    // TODO: Implement scriptNewPos logic
                    break;

                // Placeholder for scriptNewRootPos(int script, float x, float y)
                case 97: // scriptNewRootPos
                    // TODO: Implement scriptNewRootPos logic
                    break;

                // Placeholder for ins_98()
                case 98: // ins_98
                    // TODO: Unknown instruction
                    break;

                // Placeholder for ins_99(int enable)
                case 99: // ins_99
                    // TODO: Unknown instruction
                    break;

                // Placeholder for moveBezier(int t, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
                case 100: // moveBezier
                    // TODO: Implement Bezier curve movement
                    break;

                // Placeholder for ins_101()
                case 101: // ins_101
                    // TODO: Unknown instruction
                    break;

                // Placeholder for spriteRand(int a, int b)
                case 102: // spriteRand
                    // TODO: Implement spriteRand logic
                    break;

                // Placeholder for drawRect(float w, float h)
                case 103: // drawRect
                    // TODO: Implement drawRect logic
                    break;

                default:
                    // Unknown opcode
                    break;
            }
            currentInstruction = (AnmRawInstruction*)((char*)beginningOfScript + instr->offsetToNextInstr);
        }
        // TODO: Update timers and interpolation
    }
}