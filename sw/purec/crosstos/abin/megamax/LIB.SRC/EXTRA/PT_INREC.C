
int
pt_inrect(x,y, rx,ry,rw,rh)
int x,y,rx,ry,rw,rh;
{
	return (x>=rx && y>=ry && x<rx+rw && y<ry+rh);
}
