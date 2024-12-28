float average(float arr[], int size) {
    float sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum / size;
}

float sum(float arr[], int size) {
    float sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}


float sumAbs(float arr[], int size) {
    float sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += fabs(arr[i]);
    }
    return sum;
}


void squareElements(float arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = pow(arr[i], 2);
    }
}

