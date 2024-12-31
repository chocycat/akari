[← Home]('../README.md') | [Goals →](./goals.md)

---

# What is Akari?

**Akari** is a display system that was born from the lack of technological innovation in the base of the Linux desktop.

Akari is meant to be a [greenfield project](https://en.wikipedia.org/wiki/Greenfield_project). It is building something completely new while recyling some concepts that made other display servers good. _It is not a drop-in replacement for X or Wayland._

Akari puts emphasis on modern hardware and minimalism. The project is not afraid to make breaking changes or drop historical baggage.

## Why not improve X or Wayland?

**X is over 30 years old.**

X is an incredibly complex and architecturally bloated piece of software. It has years upon years of legacy burden that it cannot throw away - which is fine.

Many modern features (like HDR, HiDPI) were declared to never arrive on X, and while it's a go-to option for stability, it will sadly never be able to carry the future of the Linux desktop.

It's stable, it works (for now), but it's not future-proof.

**Wayland has fundamental design limitations.**

Wayland's design choices have been negatively influenced by a complete disregard for any advancements X has made.

The entire protocol is fragmented because compositors do all the heavy lifting.  
Each compositor must implement core functionality on its own. This fragments development efforts and puts smaller teams at a severe disadvantage.

Wayland fails to provide a practical foundation for compositors. The reference implementation, Weston, leaves out critical features like input and window positioning up to individual compositors. `libweston`, intended to be a library for reusable compositor functionality, currently labels itself as "an amalgam of various APIs mashed together."[^1]

Wayland's security model often restricts legitimate use cases without providing security benefits. The security model is particularly hostile to people who need accessibility features to use their computer.
For example, Wayland deems "a window that sets its own position" or "a window that queries its own or other windows' positions" to be security risks.

A large portion of security mitigations are based on unrealistic assumptions about the attack surface. On a typical Linux system, all user programs run in the same context anyway, making display server restrictions completely ineffective.

Most of these problems stem from the over-abstraction of Wayland. Rather than providing standardized solutions for common desktop features, Wayland delegates these responsibilities to compositors, resulting in inconsistent implementation and a fragmented user experience.

[^1]: https://wayland.pages.freedesktop.org/weston/toc/libweston.html#further-work

## Akari's approach

Akari takes a fundamentally different path:

**Do one thing well.** Akari is a display system, not an abstract protocol. It provides concrete solutions for real desktop needs.

**Keep it practical.** Build what works, not what looks good on paper. Every feature has a clear, practical purpose.

**Embrace modernity.** No compromises for legacy support. Modern computers should have modern software.

**Standard features are core features.** Common functionality belongs in the core system, not individual implementations.

**Trust the user.** Users should be able to comprehend and control their system. Security exists to protect users, not restrict them.

**Fix what's broken.** The project maintains the right to break compatibility when better solutions are found.

Akari exists to provide a new perspective on display systems, not replace them.

This means Akari won't fit everyone's needs - and that's okay. It's made for users who value simplicity, performance and direct control.

---

[← Home]('../README.md') | [Goals →](./goals.md)
