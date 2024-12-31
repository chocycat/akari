[← Goals]('../goals.md')

---

# Akari's Architecture

Akari is structured around several key components that work together to create a cohesive display system.

Akari has adopted some terminology that may be unknown to readers:

- **Clients** is a collective term for the _controller_, _apps_ and _managers_.
- **Display System** refers to the environment Akari is in as a whole. This includes all of the clients.

Below is an overview of the main components, their functions and how they interact with eachother.

## Controller

The **controller** is the heart of Akari. It acts as the central authority that coordinates interactions within the system.

- **State Management**: The controller maintains the absolute state of essential elements (for example, windows).
- **Permissions**: The controller handles permission requests from clients. When a client requests access to specific resources (for example, global input or a specific window's framebuffer), the controller evaluates these requests by either:
  - following user-defined permissions (if the permissions exist)
  - asking the user to grant or decline permissions for the client (if the permissions don't exist)
- **Transport**
  - The controller creates communication channels (sockets) between itself and each client. This is used as the primary messaging channel.
  - The controller creates beams, which is a shared memory channel. This will be expanded upon below.

## Apps

**Apps** are programs that connect to the Akari controller and use it to present windows. An app is responsible for syncing its internal state with the one present on the controller.

- **Window Management**: An application can manage their own windows, but they must rely on the controller for state updates. They send requests to create, modify or destroy windows as needed.
- **Permissions**: An application can request permissions for global input, specific windows' framebuffers, or the entire screen's framebuffer.
- **Input**: Applications handle input internally. Input data is sent to applications via a beam.

## Managers

**Managers** are specialized components that handle specific tasks within Akari. They do not communicate directly with each other but interact with apps through the controller. The task managers perform is ambigious.

- **Permissions**: A manager may request permissions for global input, specific windows' framebuffers, or the entire screen's framebuffer.

For Akari to become usable, one must use a window manager and an input manager. Other functionality may be implemented by misc. managers.

## Beams

**Beams** are a communication mechanism in Akari. It is created to facilitate fast data exchange between apps and managers. They utilize memory-mapped files to enable zero-copy data sharing.

1. All clients have an _input beam_ shared with the controller.
2. Any client may create _output beams_. A client may only create 16 of these at once.
3. Data wrote to _output beams_ is mapped to the controller.
4. The controller may perform checks or validation depending on the message type.
   - For example, if input data is sent to an output beam, the controller will route it correctly based on permissions.
5. All clients who have passed the checks or validations will get the data mapped to their _input beams_.

---

[← Goals]('../goals.md')
