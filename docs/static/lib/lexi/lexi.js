class Lexi {
    constructor(src, rules, callback) {
        this.module = null
        this.loadModule(src, rules, callback)
    }

    loadModule = (src, rules, callback) => {
        createModule()
        .then( Module => {
            // allocate src mem
            const srcLen = Module.lengthBytesUTF8(src) + 1;
            const srcPtr = Module._malloc(srcLen);
            Module.stringToUTF8(src, srcPtr, srcLen);

            // init tokenizer
            this.tokenizer = Module._initTokenizer() 

            // build rules
            Array.from( rules ).forEach(rule => {
                this.addRule( Module, rule[0], rule[1] )
            })

            // tokenize and serialize tokens
            let root = this.tokenize( Module, srcPtr )
            let tokens = []

            while ( root ) {
                tokens.push({
                    name: this.getTokenType( Module, root ),
                    start: this.getStart( Module, root ),
                    end: this.getEnd( Module, root )
                })

                root = this.getNext( Module, root )
            }

            // callback with access to tokens
            callback( tokens )
        })
        .catch( err => {
            console.error( "Lexi module failed to load", err )
        })
    }

    getNext = ( module, tokenPtr ) => {
        const getNextFunc = module.cwrap('getNext', 'number', ['number'])

        return getNextFunc( tokenPtr )
    }

    getTokenType = ( module, tokenPtr ) => {
        const getTokenTypeFunc = module.cwrap('getTokenType', 'string', ['number', 'number'])

        return getTokenTypeFunc( this.tokenizer, tokenPtr )
    }

    getStart = ( module, tokenPtr ) => {
        const getStartFunc = module.cwrap('getStart', 'int', ['number'])

        return getStartFunc( tokenPtr )
    }

    getEnd = ( module, tokenPtr ) => {
        const getEndFunc = module.cwrap('getEnd', 'int', ['number'])

        return getEndFunc( tokenPtr )
    }

    tokenize = ( module, srcPtr ) => {
        const tokenizeFunc = module.cwrap('tokenize', 'number', ['number', 'number']);

        return tokenizeFunc( this.tokenizer, srcPtr )
    }

    addRule = (module, name, expr) => {
        const addRuleFunc = module.cwrap('addRule', 'void', ['number', 'number', 'number']);

        // allocate name string
        const nameLen = module.lengthBytesUTF8(name) + 1;
        const namePtr = module._malloc(nameLen);
        module.stringToUTF8(name, namePtr, nameLen);

        // allocate expr string
        const exprLen = module.lengthBytesUTF8(expr) + 1;
        const exprPtr = module._malloc(exprLen);
        module.stringToUTF8(expr, exprPtr, exprLen);

        addRuleFunc(this.tokenizer, namePtr, exprPtr);

        // deallocate string memory
        module._free(namePtr);
        module._free(exprPtr);
    }
}