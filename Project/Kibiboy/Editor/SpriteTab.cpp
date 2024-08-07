// Includes
#include "SpriteTab.h"
#include "../Input/Input.h"
#include <iostream>
#include <algorithm>


Sprite eraserIcon = Sprite({
	0x0, 0x60, 0xf0, 0x1f8, 0x3fc, 0x7fe, 0xfff, 0x1fff,
	0x2ffe, 0x47fc, 0x83f8, 0x81f0, 0x40e0, 0x2040, 0x1080, 0xf00 });

Sprite gridIcon = Sprite({
	0xffff, 0x8421, 0x8421, 0x8421, 0x8421, 0xffff, 0x8421, 0x8421,
	0x8421, 0x8421, 0xffff, 0x8421, 0x8421, 0x8421, 0x8421, 0xffff });

Sprite trashIcon = Sprite({
	0x3c0, 0x0, 0xffff, 0x0, 0x7ffe, 0x7ffe, 0x73ce, 0x718e,
	0x381c, 0x3c3c, 0x3c3c, 0x381c, 0x318c, 0x13c8, 0x1ff8, 0x1ff8 });


void updateSpriteTab(EditorState* editor, Cart* cart){
	// Get sprite
	Sprite* openedSprite = &cart->sprites[editor->selectedSprite];
	if (openedSprite == NULL) {
		cart->sprites[editor->selectedSprite] = Sprite();
		openedSprite = &cart->sprites[editor->selectedSprite];
	}

	// Sprite pixel editing
	int pixX = std::max(0, std::min(mouseX / 8, 15));
	int pixY = std::max(0, std::min((mouseY - 32) / 8, 15));

	if (hovering(0, 32, 16 * 8, 16 * 8) && mouseDown) {
		bool drawMode = !editor->isSpriteEraserOn != rightMouseDown;
		openedSprite->setPixel(drawMode, pixX , pixY);
	}

	// Eraser toggle
	else if (hovering(16 * 8, 32, 32, 32) && justClicked) {
		editor->isSpriteEraserOn = !editor->isSpriteEraserOn;
	}

	// TODO: Rectangle tool
	// Rectangle toggle
	else if (hovering(16 * 8, 64, 32, 32) && justClicked) {
		if (editor->spriteTool == TOOL_PENCIL) {
			std::cout << "RECT\n";
			editor->spriteTool = TOOL_RECT;
		}
		else {
			editor->spriteTool = TOOL_PENCIL;
			std::cout << "PENCIL\n";
		}
	}

	// Canvas grid toggle
	else if (hovering(16 * 8, 96, 32, 32) && justClicked) {
		editor->isSpriteGridOn = !editor->isSpriteGridOn;
	}

	// Delete sprite
	else if (hovering(16 * 8, 128, 32, 32) && justClicked) {
		*openedSprite = Sprite();
	}

	// Sprite selection
	else if (hovering(160, 32, 16 * 8, 16 * 8) && justClicked) {
		int x = (mouseX - 160) / 32;
		int y = (mouseY - 32) / 32;
		int index = (editor->spriteSection * 16) + (y * 4) + (x % 4);
		editor->selectedSprite = index;
		openedSprite = &cart->sprites[editor->selectedSprite];
	}

	// Section selection
	else if (hovering(16 * 18, 32, 32, 16 * 8) && justClicked) {
		int x = (mouseX - 16 *18) / 16;
		int y = (mouseY - 32) / 16;
		int index = (y * 2) + (x % 2);
		editor->spriteSection = index;
	}

	// Color selection
	else if (hovering(16 * 12, 160, 128, 64) && justClicked) {
		int x = (mouseX - 16 * 12) / 16;
		int y = (mouseY - 160) / 16;
		int index = (y * 8) + (x % 8);

		// Set canvas color if selecting from top 16 colors
		if (index < 16) {
			editor->canvasPreviewColor = index % 16;
		}
		// Set sprite color if selecting from bottom 16 colors
		else {
			editor->spritePreviewColor = index % 16;
		}
	}

	// Sprite export (Made for debugging and engine dev only)
	// Only exports the first page (first 16 sprites)
	if ((isLCtrlDown || isRCtrlDown) && keyPress(SDLK_e)) {
		std::cout << "Exporting Sprites...\n";

		// Iterate over the first 16 sprites in the cart
		for (int i = 0; i < 16; i++) {
			Sprite sprite = cart->sprites[i];
			// Start
			std::cout << "Sprite({";

			// Export every pixel row in hex form, followed by a comma
			for (int row = 0; row < 16; row++) {
				std::cout << "0x" << std::hex << sprite.pixelRows[row];
				if (row < 15) {
					std::cout << ", ";
				}
			}
			// End
			std::cout << "}), \n";
		}
	}

	// Font export (Made for debugging and engine dev only)
	// Exports all 256 sprites using font formatting
	// The characters should only use the RIGHT 8 of their 16 pixels
	if ((isLCtrlDown || isRCtrlDown) && keyPress(SDLK_t)) {
		std::cout << "Exporting font...\n";

		// Iterate over every sprite in the cart
		for (int i = 0; i < 256; i++) {
			std::string characters =
				"!?\"#$%&'()*+,-./"
				"1234567890:;<=>@"
				"ABCDEFGHIJKLMNOP"
				"QRSTUVWXYZ[\\]^_`"
				"abcdefghijklmnop"
				"qrstuvwxyz{|}~��";

			Sprite sprite = cart->sprites[i];

			if (i > characters.size()) {
				break;
			}

			// Start
			std::cout << "{'" << characters[i] << "', Character({";

			// Export every pixel row in hex form, followed by a comma
			for (int row = 0; row < 16; row++) {
				std::cout << "0x" << std::hex << sprite.pixelRows[row];
				if (row < 15) {
					std::cout << ", ";
				}
				if (row == 7) {
					std::cout << "\n";
				}
			}
			// End
			std::cout << "})}, \n";
		}
	}

	// Cursor
	if (hovering(0, 32, 16 * 8, 16 * 8)) {
		editor->cursor = CURSOR_BRUSH;
	}
	else {
		editor->cursor = CURSOR_POINT;
	}

	// Sprite information
	editor->footerText = "X:" + std::to_string(pixX) +
		"  Y:" + std::to_string(pixY);
}


void drawSpriteTab(EditorState* editor, Cart* cart, Canvas* canvas){
	// Sprite canvas background
	canvas->rect(editor->canvasPreviewColor, 0, 32, 16 * 8, 16 * 8);

	// Sprite selection menu
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			int index = (editor->spriteSection * 16) + (y * 4) + (x % 4);

			Color iconColor = WHITE;
			Color lineColor = WHITE;
			Color rectColor = BLACK;

			if (index == editor->selectedSprite) {
				iconColor = YELLOW;
				lineColor = YELLOW;
				rectColor = BLACK;
			}
			else {
				iconColor = WHITE;
				lineColor = BLACK;
				rectColor = BLUE;
			}
			canvas->rect(lineColor, x * 32 + 160, y * 32 + 32, 32, 32);
			canvas->rect(rectColor, x * 32 + 162, y * 32 + 34, 28, 28);
			canvas->stamp(
				cart->sprites[index],
				iconColor, x * 32 + 160 + 8, y * 32 + 32 + 8);
		}
	}

	// Section selection menu
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 8; y++) {
			Color color = BLUE;
			int index = (y * 2) + (x % 2);
			if (index == editor->spriteSection) {
				color = YELLOW;
			}
			else if ((x % 2 == 0 && y % 2 == 1) ||
				(y % 2 == 0 && x % 2 == 1)) {
				color = PURPLE;
			}
			
			canvas->text(numberAsText(index, 2), color,
				x * 16 + 16 * 18, y * 16 + 32);
		}
	}

	// Tool selection menu
	canvas->stamp(eraserIcon, editor->isSpriteEraserOn? YELLOW : BLUE,
		16 * 8 + 8, 40);
	canvas->stamp(hamsterSprite,
		editor->spriteTool == TOOL_RECT ? YELLOW : BLUE,
		16 * 8 + 8, 72);
	canvas->stamp(gridIcon,
		editor->isSpriteGridOn ? YELLOW : BLUE,
		16 * 8 + 8, 104);
	canvas->stamp(trashIcon, RED, 16 * 8 + 8, 136);

	// Color selection menu
	canvas->text("CANVAS\nPREVIEW\nSPRITE\nPREVIEW", BLUE, 16 * 8, 160);
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 4; y++) {
			int index = (y * 8) + (x % 8);
			canvas->rect(Color(index),
				x * 16 + 16 * 12 + 2, y * 16 + 162, 12, 12);
			
			// Selected colors
			if (index % 16 == editor->canvasPreviewColor && index < 16) {
				canvas->stamp(selector,
					index % 16 == 0 ? WHITE : BLACK,
					x * 16 + 16 * 12, y * 16 + 160);
			}
			if (index % 16 == editor->spritePreviewColor && index > 15) {
				canvas->stamp(selector,
					index % 16 == 0 ? WHITE : BLACK,
					x * 16 + 16 * 12, y * 16 + 160);
			}
		}
	}

	// Get sprite
	Sprite* selected = NULL;
	selected = &cart->sprites[editor->selectedSprite];
	if (selected == NULL) {
		return;
	}

	// Sprite canvas grid
	if (editor->isSpriteGridOn) {
		for (int x = 0; x < 5; x++) {
			canvas->rect(
				editor->spritePreviewColor, x * 32, 32, 1, 16 * 8);
		}
		for (int y = 0; y < 5; y++) {
			canvas->rect(
				editor->spritePreviewColor, 0, y * 32 + 32, 16 * 8, 1);
		}
	}

	// Sprite at 8x size
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			if (selected->getPixel(x, y)) {
				canvas->rect(
					editor->spritePreviewColor, x * 8, y * 8 + 32, 8, 8);
			}
		}
	}

	// Sprite index
	if (editor->selectedSprite == 0) {
		canvas->text(numberAsText(editor->selectedSprite, 3),
			WHITE, 0, 16 * 8 + 32);
	}
	else {
		canvas->text(numberAsText(editor->selectedSprite, 3),
			WHITE, 0, 16 * 8 + 32);
	}
	
}