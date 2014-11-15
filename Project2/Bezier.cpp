
GLint n=4; 
GLint fill=1;
GLint TOL=10;
#define sqr(x) ((x)*(x))
GLdouble tolerance = sqr(TOL);
typedef struct { GLfloat x,y,z; } point3D_type;
Eu
GLint viewport[4];
GLdouble mvmatrix[16], projmatrix[16];
  

float vAng=40.0f, asp=1.0f, nearD=0.2f, farD=40.0f;

//GLfloat ctrlpoints[4][4][3] =
point3D_type ctrlpoints[4][4] =

{
    {
        {-1.5, -1.5, 4.0},
        {-0.5, -1.5, 2.0},
        {0.5, -1.5, -1.0},
        {1.5, -1.5, 2.0}},
    {
        {-1.5, -0.5, 1.0},
        {-0.5, -0.5, 3.0},
        {0.5, -0.5, 0.0},
        {1.5, -0.5, -1.0}},
    {
        {-1.5, 0.5, 4.0},
        {-0.5, 0.5, 0.0},
        {0.5, 0.5, 3.0},
        {1.5, 0.5, 4.0}},
    {
        {-1.5, 1.5, -2.0},
        {-0.5, 1.5, -2.0},
        {0.5, 1.5, 0.0},
        {1.5, 1.5, -1.0}}
};

 



//atenção: n é o grau do polinomio.
void Casteljau(GLfloat t, point3D_type a[], point3D_type b[], point3D_type c[], int n)
{int i,j; GLfloat  t_1=1-t;
	for (i=0; i<=n; i++)
		c[i]=a[i];
	b[0]=a[0];
	for (j=1; j<=n; j++)
	{  for (i=0; i<=n-j; i++)
	{       c[i].x=t_1*c[i].x+t*c[i+1].x;
			c[i].y=t_1*c[i].y+t*c[i+1].y;
			c[i].z=t_1*c[i].z+t*c[i+1].z;
	}
	    b[j]=c[0];
	}
	
}

void SaveBezier(char *file_name,  point3D_type a[], GLint n, GLint m)
{  FILE *file;
   file=fopen(file_name,"w");
   fprintf(file,"%d %d\n",n,m);
   for (int j=0; j<m; j++)
   {
	   for (int i=0; i<n; i++) 
    fprintf(file,"%f %f %f , ",a[j*n+i].x, a[j*n+i].y, a[j*n+i].z);
	   fprintf(file,"\n");
   }
	fclose(file);
}
void LoadBezier(char *file_name,  TO DO )
{  FILE *file;
   file=fopen(file_name,"r");
   TO DO 
   }
	fclose(file);
}
void BezierSubdivision(GLfloat u, GLfloat v, point3D_type a[],
	point3D_type b[], point3D_type c[], point3D_type d[], point3D_type e[], 
	GLint n, GLint m )
{   point3D_type *col_a = new point3D_type[m];
	point3D_type *col_b = new point3D_type[m];
	point3D_type *col_c = new point3D_type[m];
   int i,j;
   for (i=0; i<n; i++)
   {  for (j=0; j<m; j++)
        col_a[j]=a[j*n+i];
		Casteljau(v,col_a,col_b,col_c,m-1);
      for (j=0; j<m; j++)
		  {
			b[j*n+i]=col_b[j];
			c[j*n+i]=col_c[j];
		  }
   }
      
   for (j=0; j<m; j++)
   {  
	   Casteljau(u,&b[j*n],&b[j*n],&d[j*n],n-1);
	   Casteljau(u,&c[j*n],&c[j*n],&e[j*n],n-1);
   }
  

}

void DrawBezier(point3D_type a[], GLint n, GLint m, GLint nn, GLint mm)
{
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, n, 0, 1, n*3, m, &(GLfloat &)a[0]);
	 glMapGrid2f(n, 0.0, 1.0, m, 0.0, 1.0);
	if (fill==1)
    	glEvalMesh2(GL_FILL, 0, nn, 0, mm);
	else glEvalMesh2(GL_LINE, 0, nn, 0, mm);
}

bool Limit(point3D_type a[], GLint n, GLint m)
{  int i, j; GLdouble x1,y1,z1,x2,y2,z2,max=0.0, d;
	// analisar os pares (j,i) (j,i+1)
	  for (j=0; j<m; j++)
	    for (i=0; i<n-1; i++)
	  {  // projetar os pontos de controle (j,i) e (j,i+1)
           gluProject( (GLdouble) a[j*n+i].x, (GLdouble) a[j*n+i].y,  (GLdouble) a[j*n+i].z, 
			   mvmatrix, projmatrix, viewport, &x1,&y1,&z1);
		    gluProject( (GLdouble) a[j*n+i+1].x, (GLdouble) a[j*n+i+1].y,  (GLdouble) a[j*n+i+1].z, 
			   mvmatrix, projmatrix, viewport, &x2,&y2,&z2);
			d=sqr(x2-x2)+sqr(y2-y1);
			if (d>max)
				max=d;
	  }
	  
TO DO fazer o mesmo na outra direção (j,i) (j+1,i)

   return (max<=tolerance);
			 
           
}

void BezierRecursiveSubdivision(point3D_type a[], GLint n, GLint m)
{  
	 
	
	if (Limit(a,n,m))
		DrawBezier(a,n,m);
	else { 
	point3D_type *b=new point3D_type[m*n];
	point3D_type *c=new point3D_type[m*n];
	point3D_type *d=new point3D_type[m*n];
	point3D_type *e=new point3D_type[m*n];
		BezierSubdivision(0.5f,0.5f,a,b,c,d,e,n,m);
	
	if (Limit(b,n,m))
		DrawBezier(b,n,m,n,m);
	else BezierRecursiveSubdivision(b,n,m);
	TO DO chamar recursivamente para os ptos de controle c, d, e

	}
}

 void initlights(void)
{
    GLfloat ambient[] =
    {0.1, 0.7, 0.7, 1.0};
    GLfloat position[] =
    {0.0, 0.0, 3.0, 2.0};
    GLfloat mat_diffuse[] =
    {0.1, 0.9, 0.9, 1.0};
    GLfloat mat_specular[] =
    {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] =
    {50.0};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}


GLfloat texpts[2][2][2] = {{{0.0, 0.0}, {0.0, 1.0}}, 
                        {{1.0, 0.0}, {1.0, 1.0}}};



   
void display(void)
{  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMapGrid2f(n, 0.0, 1.0, n, 0.0, 1.0);
	glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
	 
    glRotatef(85.0, 1.0, 1.0, 1.0);
	
	if (perspectiva)
	{  tolerance = sqr(TOL*2);
	 gluPerspective(vAng,asp,nearD,farD); 
	//   cam.Set(eye1,look,up);

	  cam.Update();
	}
	
	
  
	BezierRecursiveSubdivision((point3D_type *)ctrlpoints, 4,4);
    glPopMatrix();

    glFlush();
    glutSwapBuffers(); // display the screen just made 
	
	
	
}


void myinit(void)
{   
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &(GLfloat &)ctrlpoints);
    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
	 

               
    glMapGrid2f(n, 0.0, 1.0, n, 0.0, 1.0);
    initlights();       /* for lighted version only */
}

void myReshape(int w, int h)
{  GLfloat f=1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	if (perspectiva)
	{
	 gluPerspective(vAng,asp,nearD,farD); 
	 cam.Set((CPoint3D)eye1,(CPoint3D)look,(CPoint3D)up);
	 cam.Update();
	}
	else {
    if (w <= h)
        glOrtho(-4.0, 4.0, -4.0 * (GLfloat) h / (GLfloat) w,
            4.0 * (GLfloat) h / (GLfloat) w, -4.0, 4.0);
    else
        glOrtho(-4.0 * (GLfloat) w / (GLfloat) h,
            4.0 * (GLfloat) w / (GLfloat) h, -4.0, 4.0, -4.0, 4.0);
	}


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	//cam.setAspect(((float)w)/h);
	glGetIntegerv (GL_VIEWPORT, viewport);
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
}




 
   

void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {       /*  increase n */
        //   Put code here
           
            glutPostRedisplay();
         }
         break;
      
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {      /*  change blue  */
       //   Put code here
           
            glutPostRedisplay();
         }
         break;
      default:
         break;
   }
}

     
        

     
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    myinit();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboard);
	
	glutMouseFunc(mouse);
    glutMainLoop();
    return 0;             /* ANSI C requires main to return int. */
}
