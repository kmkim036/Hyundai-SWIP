// game 1
while (1) {
    measure light sensor;
    LED random on;  // light sensor % 3 == 0, 1, 2
    // 0 <= LED1
    // 1 <= LED2
    // 2 <= LED1, LED2
    timer start;
    if (timer > 1.5s || uncorrect switch) {
        // no action
    } else {
        win_cnt++
    }
    switch (win_cnt) {
    case 0:
        no rgb led;
        break;
    case 1:
        red rgb led;
        break;
    case 2:
        green rgb led;
        break;
    case 3:
        blue rgb led;
        break;
    case 4:
        white rgb led;
        break;
    }
    while (1)
        ;
}

// game 2
mode = 1 or 0;

while (1) {
    while (1) {
        if (mode == 1) {
            RGB GREEN;
        if (reached?) {
            game win;
            LED BLUE;
            break;
        }
        } else {
            save distance;
            RGB RED;
            if (distance changed?) {
                game lose;
                LED RED;
                break;
            }
        }
    }
    while (1)
        ;
}

timer interrupt
{
    mode toggle;
    3 seconds;
}

// game 3
while (1) {
    measure light sensor;
    random target potentiometer setting;
    while (1) {
        if (potentiometer > target value) {
            RGB BLUE;
        } else {
            RGB RED;
            break;
        }
    }
    while (1)
        ;
}
