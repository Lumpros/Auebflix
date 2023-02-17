#pragma once

// Step 1: MessageGenerator::generateMessages is called during the update function of the sgg window.
//         This method may allocate some data on the heap so we must deallocate the data later.
//
// Step 2: Each message generated is added to the message queue of the top level window using postMessage
//
// Step 3: The update function of the top level window is called, which removes the oldest message from
//         from the queue and forwards it to the appropriate window using Messenger::forwardMessage
//
// Step 4: MessageGenerator::deallocateMessage is called in order to potentially deallocate memory

struct Message
{
	enum class Code {
		LEFT_MOUSE_DOWN,  // data: Pointer to a Point struct containing the point that was left clicked in Window coordinates
		LEFT_MOUSE_UP,    // data: Pointer to a Point struct containing the point where the left mouse was released in Window coordinates
		RIGHT_MOUSE_DOWN, // data: Pointer to a Point struct containing the point that was right clicked in Window coordinates
		RIGHT_MOUSE_UP,   // data: Pointer to a Point struct containing the point where the right mouse was released in Window coordinates
		MOVING_MOUSE,     // data:
		MOUSE_LEFT,
		MOUSE_ENTER,
		KEY_PRESSED,      // data: Pointer to a KeyMessageInfo struct containing information about the key press
		KEY_RELEASED,     // data: Pointer to a KeyMessageInfo struct containing information about the key release
		WINDOW_RESIZED,   // data: Pointer to a Size struct containing the new size
		GOT_FOCUS,        // data: NULL
		LOST_FOCUS,       // data: NULL
		TIMER,
		CUSTOM
	} code;

	void* data;
};