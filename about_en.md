# Technical Details: Prepared Collapse of Order Model (PCM)

## What is the "Prepared Collapse of Order" Model?
The **"Prepared Collapse of Order"** model is an architectural design that allows a stack frame to be destroyed and discarded with maximum efficiency and safety by leveraging intentional data corruption.

---

## Common Models
Typically, discarding a stack frame requires temporary storage because two distinct values must be managed during the transition:
1. The **Return Value**
2. The **Return Address / Parent Frame Information**



---

## The PCM Approach
In this model, no temporary storage is required, and memory efficiency is significantly improved.



Consider a memory region `A`, where the parent frame and the current (child) frame overlap.
- The **Parent Frame** expects the **Return Value** to appear at position `A`.
- The **Current Frame** uses position `A` to store its own **Instruction Pointer (IP)** (the current bytecode index).

Let's assume the top of the current frame holds the return value `B`:
```
| Parent Frame : A (Instruction Pointer) : Current Frame, Return Value(=B) |
```
When the current frame is discarded:
1. The Return Value `B` is written directly into position `A` (effectively overwriting the IP).
2. The frame pointer is switched back to the parent.
```
| Parent Frame, B (now interpreted as Return Value) |
```
---

## Etymology
The term **"Prepared Collapse of Order"** stems from the fact that we intentionally violate the integrity of the operand stack size and its reality when stacking frames. By driving the frame into a "technically illegal" state during destruction, we achieve a safe and instantaneous transition within a controlled execution window.

---

## Safety
Why is it safe to overwrite the active Instruction Pointer? This is guaranteed by the execution cycle of **YSVM**.

YSVM increments the "current bytecode index" **after** a command has finished its execution logic. Since the `RET` command signifies the absolute end of the frame’s utility, the IP at that moment becomes "obsolete data." Overwriting this meaningless data with the Return Value is a safe operation, provided the frame switch occurs immediately before the next fetch cycle.



---

## Efficiency
By eliminating the need for temporary variables, this model operates more efficiently than traditional designs.

| Step | Standard Model | YSVM (PCM) |
| :--- | :--- | :--- |
| 1 | Evacuate return value to temp slot | **Overwrite IP with return value** |
| 2 | Discard current frame | **Discard current frame** |
| 3 | PUSH return value to parent stack | (Already completed in Step 1) |

---

**Proposed by:** 87right  
**First Edition:** January 22, 2026