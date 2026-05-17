#pragma once

void drv8871_init(void);
void drv8871_set(int percent);  /* -100 (full reverse) … +100 (full forward); 0 = coast */
void drv8871_brake(void);       /* active brake: both inputs high */
