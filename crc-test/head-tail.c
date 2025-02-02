char * bump_head(char * head, char * endofbuf,char * topofbuf) {
 
	if(head == endofbuf) {

		
			//printf("head == endofbuf\n");
			head = topofbuf;
	}
	else {
		//printf("head < endofbuf\n");
		head = head + 1;
	}
 
	
	return((char *)head);
}
char * bump_tail(char * tail,char * endofbuf,char * topofbuf) {
	
	if(tail == endofbuf) {

		
			//printf("tail == endofbuf\n");
			tail = topofbuf;
	}
	else {
		//printf("tail < endofbuf\n");
		tail = tail + 1;
	}
 
	
	return((char *)tail);
}
char * dec_head(char * head,char * endofbuf,char * topofbuf) {
	if(head == topofbuf) {
			//printf("head == topofbuf\n");
			//head = topofbuf;
	}
	else {
		//printf("head < topofbuf\n");
		head = head - 1;
	}

	return((char *)head);
}
char * dec_tail(char * tail,char * endofbuf,char * topofbuf) {
	if(tail == topofbuf) {
			printf("tail == topofbuf\n");
			//head = topofbuf;
	}
	else {
		printf("tail < topofbuf\n");
		tail = tail - 1;
	}

	return((char *)tail); 
}
