class a081e536ec67{

    constructor(){
        this.canvas = document.getElementById('a081e536ec67');
        this.startStopButton = document.getElementById('startStop_a081e536ec67');
        this.stepButton = document.getElementById('step_a081e536ec67');
        this.ctx = this.canvas.getContext('2d');

        this.resolution = 3;

        this.rows = 128;
        this.cols = 128;

        this.running = false;

        this.canvas.height  = this.resolution * this.rows;
        this.canvas.width   = this.resolution * this.cols;

        this.carrying_capacity_predator = 5.0;
        this.carrying_capacity_prey = 30.0;

        this.delta_t = 0.01;

        this.a = 1.001;
        this.b = 0.4;
        this.c = 0.1;
        this.d = 0.5;

        this.redraw = true;
        
    }

    initGrid(default_value){
        // Allocate the prey grid
        //this.grid = new Array(this.rows).fill(null);
        //for (let i=0; i<this.grid.length; i++){
        //    this.grid[i] = new Array(this.cols).fill(null).map(() => Math.floor(2.0*Math.random()));
        //}

        var grid = new Array(this.rows).fill(null);
        for (let i=0; i<grid.length; i++){
            grid[i] = new Array(this.cols).fill(null).map(() => Math.floor(default_value));
        }
        
        return grid;
    }
  
    render(){
        for (let r = 0; r<this.rows; r++){
            for (let c = 0; c < this.cols; c++){
                
                const predator_pop = this.g_predator[r][c];
                const prey_pop     = this.g_prey[r][c];
                
                this.ctx.beginPath();
                this.ctx.strokeStyle = '#8e8e8e';
                this.ctx.fillStyle = '#' + '00' + this.int_to_hex((predator_pop/this.carrying_capacity_predator)*255)  + this.int_to_hex((prey_pop/this.carrying_capacity_prey)*255);
                this.ctx.rect(c*this.resolution,r*this.resolution,this.resolution,this.resolution);
                this.ctx.fill();
                
            }
        }
    }

    reset(){
        // Allocate the population grids
        this.g_predator = this.initGrid(0.0);
        this.g_prey = this.initGrid(10.0);
        
        for (let i=0;i<10;i++){
            let row = Math.floor(this.rows*Math.random());
            let col = Math.floor(this.cols*Math.random());
            this.g_predator[row][col] = this.carrying_capacity_predator*Math.random();
            //this.g_predator[row][col] = 20.0*Math.random();
            this.g_prey[row][col] = this.carrying_capacity_prey*Math.random();
            //this.g_prey[row][col] = 30.0*Math.random();
        }
        
        this.render();
    }

    nextFrame(){
        for (let i=0;i<12;i++){
            this.update();
            this.migrate();

        }
        
        this.render();

        if (this.running){
            //window.setTimeout(this.nextFrame,30);
            //console.log('scheduling next frame');
            window.setTimeout(this.nextFrame.bind(this),1);
        }
    }

    startStop(){
        this.running = !this.running;
        if (this.running){
            this.startStopButton.value = "Stop";
            this.stepButton.disabled = true;
            this.nextFrame();
        }
        else{
            this.startStopButton.value = "Start";
            this.stepButton.disabled = false;
        }
    }

    update(){

        //console.log('update');
        const tmp_predator = this.g_predator.map(arr => [...arr]);
        const tmp_prey = this.g_prey.map(arr => [...arr]);

        for (let r = 0; r<this.rows; r++){
            for (let c = 0; c < this.cols; c++){

                const pop_predator = tmp_predator[r][c];
                const pop_prey = tmp_prey[r][c];

                let delta_prey     = this.a * pop_prey                - this.b * pop_prey * pop_predator;
                let delta_predator = this.c * pop_prey * pop_predator - this.d * pop_predator;

                //console.log(delta_prey);
                //console.log(delta_predator);

                this.g_predator[r][c] += delta_predator * this.delta_t;
                this.g_prey[r][c]     += delta_prey * this.delta_t;

                this.g_predator[r][c] = Math.max(this.g_predator[r][c],0.0);
                this.g_prey[r][c]     = Math.max(this.g_prey[r][c],0.0);

                
            }
        }
    }

    migrate(){
        //console.log('migrate');
        const tmp_predator = this.g_predator.map(arr => [...arr]);
        const tmp_prey = this.g_prey.map(arr => [...arr]);

        let g_tmp_predator = this.initGrid(0.0);
        let g_tmp_prey = this.initGrid(0.0);

        // Compute the migrations
        for (let r = 0; r<this.rows; r++){
            for (let c = 0; c < this.cols; c++){
                
                let migrating_predators = 0;
                let migrating_prey = 0;
                
                for (let i = -1; i<=1; i++){
                    for (let j = -1; j<=1; j++){
                        
                        let curr_row = r+i;
                        let curr_col = c+j;

                        if (curr_row < 0 || curr_row == this.rows)
                            continue;

                        if (curr_col < 0 || curr_col == this.cols)
                            continue;

                        // Need to compute the excess prey in the neighboring cell
                        migrating_prey += Math.max(tmp_prey[curr_row][curr_col]-this.carrying_capacity_prey,0.0);
                        migrating_predators += Math.max(tmp_predator[curr_row][curr_col]-this.carrying_capacity_predator,0.0);

                    }
                }

                migrating_prey /= 8.0;
                migrating_predators /= 8.0;

                g_tmp_predator[r][c] = migrating_predators;
                g_tmp_prey[r][c] = migrating_prey;
            }
        }

        // Update the totals based on migrations
        for (let r = 0; r<this.rows; r++){
            for (let c = 0; c < this.cols; c++){
                this.g_predator[r][c]  = Math.min(this.g_predator[r][c] + g_tmp_predator[r][c],this.carrying_capacity_predator);
                this.g_prey[r][c]  = Math.min(this.g_prey[r][c] + g_tmp_prey[r][c],this.carrying_capacity_prey);
            }
        }

    }

    int_to_hex(num){
        var hex = Math.round(num).toString(16);
        if (hex.length == 1)
            hex = '0' + hex;
        return hex;
    }
}

let instance_a081e536ec67 = new a081e536ec67();
instance_a081e536ec67.reset();