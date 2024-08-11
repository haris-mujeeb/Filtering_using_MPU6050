

# 1. IIR Filter (1st Order)
### Formula:
![image](https://github.com/user-attachments/assets/dd470c04-db7b-473f-ac3b-18d72fe8ff8f)

We have one zero ($\alpha$) and one pole ($\beta$). The system is stable for $|\beta| \leq 1$ or $-1 < \beta < 1$.

### Code:
```c
typedef struct {
	float alpha, beta, output;
} IIRFilter;

 void IIRFilter_Init ( IIRFilter *filt, float _alpha, float _beta)
 {
 filt->alpha = _alpha;
 filt->beta  = _beta;
 filt->output = 0.0f;
 }
 
float IIRFilter_Update ( IIRFilter *filt, float input ) {
 // yn = A * xn - B * y(n-1)
filt->output = ( filt->alpha * input ) - ( filt->beta * filt-> output );

// Return filter output
 return filt->output;
 }
```


 **Output Plots:**
- **Stable** ( -1 < Beta < 1):
![image](https://github.com/user-attachments/assets/ee8eda5a-b119-4a5f-935a-6a0ead0e815b)

- **Unstable** (  Beta < -1 or  Beta > 1):
![400](https://github.com/user-attachments/assets/2254de4d-4dd8-4b5f-a9d9-57a92a41d1ea)
