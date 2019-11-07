void pollInput() {
  // update inputs
  SDL_Event e;
  while(SDL_PollEvent(&e)){
    if(e.type == SDL_QUIT) exit(0);
  }
  const uint8_t *input_state = SDL_GetKeyboardState(NULL);

  input.shift_register1 = input.shift_register2 = 0;

  // Controller 1
  if (input_state[SDL_SCANCODE_N]) input.shift_register1 |= BIT0;
  if (input_state[SDL_SCANCODE_M]) input.shift_register1 |= BIT1;
  if (input_state[SDL_SCANCODE_RSHIFT]) input.shift_register1 |= BIT2;
  if (input_state[SDL_SCANCODE_RETURN]) input.shift_register1 |= BIT3;
  if (input_state[SDL_SCANCODE_UP]) input.shift_register1 |= BIT4;
  if (input_state[SDL_SCANCODE_DOWN]) input.shift_register1 |= BIT5;
  if (input_state[SDL_SCANCODE_LEFT]) input.shift_register1 |= BIT6;
  if (input_state[SDL_SCANCODE_RIGHT]) input.shift_register1 |= BIT7;

  // Controller 2
  if (input_state[SDL_SCANCODE_R]) input.shift_register2 |= BIT0;
  if (input_state[SDL_SCANCODE_T]) input.shift_register2 |= BIT1;
  if (input_state[SDL_SCANCODE_LSHIFT]) input.shift_register2 |= BIT2;
  if (input_state[SDL_SCANCODE_LCTRL]) input.shift_register2 |= BIT3;
  if (input_state[SDL_SCANCODE_W]) input.shift_register2 |= BIT4;
  if (input_state[SDL_SCANCODE_S]) input.shift_register2 |= BIT5;
  if (input_state[SDL_SCANCODE_A]) input.shift_register2 |= BIT6;
  if (input_state[SDL_SCANCODE_D]) input.shift_register2 |= BIT7;
}

uint8_t getNextInput1() {
  bool bit = input.shift_register1 & BIT0;
  input.shift_register1 >>= 1;
  return bit | 0b1000000;
}

uint8_t getNextInput2() {
  bool bit = input.shift_register2 & BIT0;
  input.shift_register2 >>= 1;
  return bit | 0b1000000;
}
